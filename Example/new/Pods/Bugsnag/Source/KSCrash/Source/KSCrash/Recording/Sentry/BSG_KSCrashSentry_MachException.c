//
//  BSG_KSCrashSentry_MachException.c
//
//  Created by Karl Stenerud on 2012-02-04.
//
//  Copyright (c) 2012 Karl Stenerud. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "BSG_KSCrashSentry_MachException.h"
#include "BSG_KSSystemCapabilities.h"

//#define BSG_KSLogger_LocalLevel TRACE
#include "BSG_KSLogger.h"

#if BSG_KSCRASH_HAS_MACH

#include "BSG_KSMach.h"
#include "BSG_KSCrashSentry_Private.h"
#include <pthread.h>
#include <mach/mach.h>

// ============================================================================
#pragma mark - Constants -
// ============================================================================

#define kThreadPrimary "KSCrash Exception Handler (Primary)"
#define kThreadSecondary "KSCrash Exception Handler (Secondary)"

// ============================================================================
#pragma mark - Types -
// ============================================================================

/** A mach exception message (according to ux_exception.c, xnu-1699.22.81).
 */
typedef struct {
    /** Mach header. */
    mach_msg_header_t header;

    // Start of the kernel processed data.

    /** Basic message body data. */
    mach_msg_body_t body;

    /** The thread that raised the exception. */
    mach_msg_port_descriptor_t thread;

    /** The task that raised the exception. */
    mach_msg_port_descriptor_t task;

    // End of the kernel processed data.

    /** Network Data Representation. */
    NDR_record_t NDR;

    /** The exception that was raised. */
    exception_type_t exception;

    /** The number of codes. */
    mach_msg_type_number_t codeCount;

    /** Exception code and subcode. */
    // ux_exception.c defines this as mach_exception_data_t for some reason.
    // But it's not actually a pointer; it's an embedded array.
    // On 32-bit systems, only the lower 32 bits of the code and subcode
    // are valid.
    mach_exception_data_type_t code[0];

    /** Padding to avoid RCV_TOO_LARGE. */
    char padding[512];
} MachExceptionMessage;

/** A mach reply message (according to ux_exception.c, xnu-1699.22.81).
 */
typedef struct {
    /** Mach header. */
    mach_msg_header_t header;

    /** Network Data Representation. */
    NDR_record_t NDR;

    /** Return code. */
    kern_return_t returnCode;
} MachReplyMessage;

// ============================================================================
#pragma mark - Globals -
// ============================================================================

/** Flag noting if we've installed our custom handlers or not.
 * It's not fully thread safe, but it's safer than locking and slightly better
 * than nothing.
 */
static volatile sig_atomic_t bsg_g_installed = 0;

/** Holds exception port info regarding the previously installed exception
 * handlers.
 */
static struct {
    exception_mask_t masks[EXC_TYPES_COUNT];
    exception_handler_t ports[EXC_TYPES_COUNT];
    exception_behavior_t behaviors[EXC_TYPES_COUNT];
    thread_state_flavor_t flavors[EXC_TYPES_COUNT];
    mach_msg_type_number_t count;
} bsg_g_previousExceptionPorts;

/** Our exception port. */
static mach_port_t bsg_g_exceptionPort = MACH_PORT_NULL;

/** Primary exception handler thread. */
static pthread_t bsg_g_primaryPThread;
static thread_t bsg_g_primaryMachThread;

/** Secondary exception handler thread in case crash handler crashes. */
static pthread_t bsg_g_secondaryPThread;
static thread_t bsg_g_secondaryMachThread;

/** Context to fill with crash information. */
static BSG_KSCrash_SentryContext *bsg_g_context;

// ============================================================================
#pragma mark - Utility -
// ============================================================================

// Avoiding static methods due to linker issue.

/** Get all parts of the machine state required for a dump.
 * This includes basic thread state, and exception registers.
 *
 * @param thread The thread to get state for.
 *
 * @param machineContext The machine context to fill out.
 */
bool bsg_ksmachexc_i_fetchMachineState(
    const thread_t thread, BSG_STRUCT_MCONTEXT_L *const machineContext) {
    if (!bsg_ksmachthreadState(thread, machineContext)) {
        return false;
    }

    if (!bsg_ksmachexceptionState(thread, machineContext)) {
        return false;
    }

    return true;
}

/** Restore the original mach exception ports.
 */
void bsg_ksmachexc_i_restoreExceptionPorts(void) {
    BSG_KSLOG_DEBUG("Restoring original exception ports.");
    if (bsg_g_previousExceptionPorts.count == 0) {
        BSG_KSLOG_DEBUG("Original exception ports were already restored.");
        return;
    }

    const task_t thisTask = mach_task_self();
    kern_return_t kr;

    // Reinstall old exception ports.
    for (mach_msg_type_number_t i = 0; i < bsg_g_previousExceptionPorts.count;
         i++) {
        BSG_KSLOG_TRACE("Restoring port index %d", i);
        kr = task_set_exception_ports(thisTask,
                                      bsg_g_previousExceptionPorts.masks[i],
                                      bsg_g_previousExceptionPorts.ports[i],
                                      bsg_g_previousExceptionPorts.behaviors[i],
                                      bsg_g_previousExceptionPorts.flavors[i]);
        if (kr != KERN_SUCCESS) {
            BSG_KSLOG_ERROR("task_set_exception_ports: %s",
                            mach_error_string(kr));
        }
    }
    BSG_KSLOG_DEBUG("Exception ports restored.");
    bsg_g_previousExceptionPorts.count = 0;
}

// ============================================================================
#pragma mark - Handler -
// ============================================================================

/** Our exception handler thread routine.
 * Wait for an exception message, uninstall our exception port, record the
 * exception information, and write a report.
 */
void *ksmachexc_i_handleExceptions(void *const userData) {
    MachExceptionMessage exceptionMessage = {{0}};
    MachReplyMessage replyMessage = {{0}};

    const char *threadName = (const char *)userData;
    pthread_setname_np(threadName);
    if (threadName == kThreadSecondary) {
        BSG_KSLOG_DEBUG("This is the secondary thread. Suspending.");
        thread_suspend(bsg_ksmachthread_self());
    }

    for (;;) {
        BSG_KSLOG_DEBUG("Waiting for mach exception");

        // Wait for a message.
        kern_return_t kr = mach_msg(
            &exceptionMessage.header, MACH_RCV_MSG, 0, sizeof(exceptionMessage),
            bsg_g_exceptionPort, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
        if (kr == KERN_SUCCESS) {
            break;
        }

        // Loop and try again on failure.
        BSG_KSLOG_ERROR("mach_msg: %s", mach_error_string(kr));
    }

    BSG_KSLOG_DEBUG("Trapped mach exception code 0x%x, subcode 0x%x",
                    exceptionMessage.code[0], exceptionMessage.code[1]);
    if (bsg_g_installed) {
        bool wasHandlingCrash = bsg_g_context->handlingCrash;
        bsg_kscrashsentry_beginHandlingCrash(bsg_g_context);

        BSG_KSLOG_DEBUG(
            "Exception handler is installed. Continuing exception handling.");

        BSG_KSLOG_DEBUG("Suspending all threads");
        bsg_kscrashsentry_suspendThreads();

        // Switch to the secondary thread if necessary, or uninstall the handler
        // to avoid a death loop.
        if (bsg_ksmachthread_self() == bsg_g_primaryMachThread) {
            BSG_KSLOG_DEBUG("This is the primary exception thread. Activating "
                            "secondary thread.");
            if (thread_resume(bsg_g_secondaryMachThread) != KERN_SUCCESS) {
                BSG_KSLOG_DEBUG("Could not activate secondary thread. "
                                "Restoring original exception ports.");
                bsg_ksmachexc_i_restoreExceptionPorts();
            }
        } else {
            BSG_KSLOG_DEBUG("This is the secondary exception thread. Restoring "
                            "original exception ports.");
            bsg_ksmachexc_i_restoreExceptionPorts();
        }

        if (wasHandlingCrash) {
            BSG_KSLOG_INFO("Detected crash in the crash reporter. Restoring "
                           "original handlers.");
            // The crash reporter itself crashed. Make a note of this and
            // uninstall all handlers so that we don't get stuck in a loop.
            bsg_g_context->crashedDuringCrashHandling = true;
            bsg_kscrashsentry_uninstall(BSG_KSCrashTypeAsyncSafe);
        }

        // Fill out crash information
        BSG_KSLOG_DEBUG("Fetching machine state.");
        BSG_STRUCT_MCONTEXT_L machineContext;
        if (bsg_ksmachexc_i_fetchMachineState(exceptionMessage.thread.name,
                                              &machineContext)) {
            if (exceptionMessage.exception == EXC_BAD_ACCESS) {
                bsg_g_context->faultAddress =
                    bsg_ksmachfaultAddress(&machineContext);
            } else {
                bsg_g_context->faultAddress =
                    bsg_ksmachinstructionAddress(&machineContext);
            }
        }

        BSG_KSLOG_DEBUG("Filling out context.");
        bsg_g_context->crashType = BSG_KSCrashTypeMachException;
        bsg_g_context->offendingThread = exceptionMessage.thread.name;
        bsg_g_context->registersAreValid = true;
        bsg_g_context->mach.type = exceptionMessage.exception;
        bsg_g_context->mach.code = exceptionMessage.code[0];
        bsg_g_context->mach.subcode = exceptionMessage.code[1];

        BSG_KSLOG_DEBUG("Calling main crash handler.");
        bsg_g_context->onCrash();

        BSG_KSLOG_DEBUG(
            "Crash handling complete. Restoring original handlers.");
        bsg_kscrashsentry_uninstall(BSG_KSCrashTypeAsyncSafe);
        bsg_kscrashsentry_resumeThreads();
    }

    BSG_KSLOG_DEBUG("Replying to mach exception message.");
    // Send a reply saying "I didn't handle this exception".
    replyMessage.header = exceptionMessage.header;
    replyMessage.NDR = exceptionMessage.NDR;
    replyMessage.returnCode = KERN_FAILURE;

    mach_msg(&replyMessage.header, MACH_SEND_MSG, sizeof(replyMessage), 0,
             MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

    return NULL;
}

// ============================================================================
#pragma mark - API -
// ============================================================================

bool bsg_kscrashsentry_installMachHandler(
    BSG_KSCrash_SentryContext *const context) {
    BSG_KSLOG_DEBUG("Installing mach exception handler.");

    bool attributes_created = false;
    pthread_attr_t attr;

    kern_return_t kr;
    int error;

    const task_t thisTask = mach_task_self();
    exception_mask_t mask = EXC_MASK_BAD_ACCESS | EXC_MASK_BAD_INSTRUCTION |
                            EXC_MASK_ARITHMETIC | EXC_MASK_SOFTWARE |
                            EXC_MASK_BREAKPOINT;

    if (bsg_g_installed) {
        BSG_KSLOG_DEBUG("Exception handler already installed.");
        return true;
    }
    bsg_g_installed = 1;

    if (bsg_ksmachisBeingTraced()) {
        // Different debuggers hook into different exception types.
        // For example, GDB uses EXC_BAD_ACCESS for single stepping,
        // and LLDB uses EXC_SOFTWARE to stop a debug session.
        // Because of this, it's safer to not hook into the mach exception
        // system at all while being debugged.
        BSG_KSLOG_WARN("Process is being debugged. Not installing handler.");
        goto failed;
    }

    bsg_g_context = context;

    BSG_KSLOG_DEBUG("Backing up original exception ports.");
    kr = task_get_exception_ports(
        thisTask, mask, bsg_g_previousExceptionPorts.masks,
        &bsg_g_previousExceptionPorts.count, bsg_g_previousExceptionPorts.ports,
        bsg_g_previousExceptionPorts.behaviors,
        bsg_g_previousExceptionPorts.flavors);
    if (kr != KERN_SUCCESS) {
        BSG_KSLOG_ERROR("task_get_exception_ports: %s", mach_error_string(kr));
        goto failed;
    }

    if (bsg_g_exceptionPort == MACH_PORT_NULL) {
        BSG_KSLOG_DEBUG("Allocating new port with receive rights.");
        kr = mach_port_allocate(thisTask, MACH_PORT_RIGHT_RECEIVE,
                                &bsg_g_exceptionPort);
        if (kr != KERN_SUCCESS) {
            BSG_KSLOG_ERROR("mach_port_allocate: %s", mach_error_string(kr));
            goto failed;
        }

        BSG_KSLOG_DEBUG("Adding send rights to port.");
        kr = mach_port_insert_right(thisTask, bsg_g_exceptionPort,
                                    bsg_g_exceptionPort,
                                    MACH_MSG_TYPE_MAKE_SEND);
        if (kr != KERN_SUCCESS) {
            BSG_KSLOG_ERROR("mach_port_insert_right: %s",
                            mach_error_string(kr));
            goto failed;
        }
    }

    BSG_KSLOG_DEBUG("Installing port as exception handler.");
    kr = task_set_exception_ports(thisTask, mask, bsg_g_exceptionPort,
                                  EXCEPTION_DEFAULT, THREAD_STATE_NONE);
    if (kr != KERN_SUCCESS) {
        BSG_KSLOG_ERROR("task_set_exception_ports: %s", mach_error_string(kr));
        goto failed;
    }

    BSG_KSLOG_DEBUG("Creating secondary exception thread (suspended).");
    pthread_attr_init(&attr);
    attributes_created = true;
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    error = pthread_create(&bsg_g_secondaryPThread, &attr,
                           &ksmachexc_i_handleExceptions, kThreadSecondary);
    if (error != 0) {
        BSG_KSLOG_ERROR("pthread_create_suspended_np: %s", strerror(error));
        goto failed;
    }
    bsg_g_secondaryMachThread = pthread_mach_thread_np(bsg_g_secondaryPThread);
    context->reservedThreads[BSG_KSCrashReservedThreadTypeMachSecondary] =
        bsg_g_secondaryMachThread;

    BSG_KSLOG_DEBUG("Creating primary exception thread.");
    error = pthread_create(&bsg_g_primaryPThread, &attr,
                           &ksmachexc_i_handleExceptions, kThreadPrimary);
    if (error != 0) {
        BSG_KSLOG_ERROR("pthread_create: %s", strerror(error));
        goto failed;
    }
    pthread_attr_destroy(&attr);
    bsg_g_primaryMachThread = pthread_mach_thread_np(bsg_g_primaryPThread);
    context->reservedThreads[BSG_KSCrashReservedThreadTypeMachPrimary] =
        bsg_g_primaryMachThread;

    BSG_KSLOG_DEBUG("Mach exception handler installed.");
    return true;

failed:
    BSG_KSLOG_DEBUG("Failed to install mach exception handler.");
    if (attributes_created) {
        pthread_attr_destroy(&attr);
    }
    bsg_kscrashsentry_uninstallMachHandler();
    return false;
}

void bsg_kscrashsentry_uninstallMachHandler(void) {
    BSG_KSLOG_DEBUG("Uninstalling mach exception handler.");

    if (!bsg_g_installed) {
        BSG_KSLOG_DEBUG("Mach exception handler was already uninstalled.");
        return;
    }

    // NOTE: Do not deallocate the exception port. If a secondary crash occurs
    // it will hang the process.

    bsg_ksmachexc_i_restoreExceptionPorts();

    thread_t thread_self = bsg_ksmachthread_self();

    if (bsg_g_primaryPThread != 0 && bsg_g_primaryMachThread != thread_self) {
        BSG_KSLOG_DEBUG("Cancelling primary exception thread.");
        if (bsg_g_context->handlingCrash) {
            thread_terminate(bsg_g_primaryMachThread);
        } else {
            pthread_cancel(bsg_g_primaryPThread);
        }
        bsg_g_primaryMachThread = 0;
        bsg_g_primaryPThread = 0;
    }
    if (bsg_g_secondaryPThread != 0 && bsg_g_secondaryMachThread != thread_self) {
        BSG_KSLOG_DEBUG("Cancelling secondary exception thread.");
        if (bsg_g_context->handlingCrash) {
            thread_terminate(bsg_g_secondaryMachThread);
        } else {
            pthread_cancel(bsg_g_secondaryPThread);
        }
        bsg_g_secondaryMachThread = 0;
        bsg_g_secondaryPThread = 0;
    }

    BSG_KSLOG_DEBUG("Mach exception handlers uninstalled.");
    bsg_g_installed = 0;
}

#else

bool bsg_kscrashsentry_installMachHandler(
    __unused BSG_KSCrash_SentryContext *const context) {
    BSG_KSLOG_WARN("Mach exception handler not available on this platform.");
    return false;
}

void bsg_kscrashsentry_uninstallMachHandler(void) {}

#endif
