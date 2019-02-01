//
//  BSG_KSCrashSentry.c
//
//  Created by Karl Stenerud on 2012-02-12.
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

#include "BSG_KSSystemCapabilities.h"
#include "BSG_KSCrashSentry.h"
#include "BSG_KSCrashSentry_Private.h"

#include "BSG_KSCrashSentry_CPPException.h"
#include "BSG_KSCrashSentry_Deadlock.h"
#include "BSG_KSCrashSentry_NSException.h"
#include "BSG_KSCrashSentry_MachException.h"
#include "BSG_KSCrashSentry_Signal.h"
#include "BSG_KSCrashSentry_User.h"
#include "BSG_KSLogger.h"
#include "BSG_KSMach.h"

// ============================================================================
#pragma mark - Globals -
// ============================================================================

typedef struct {
    BSG_KSCrashType crashType;
    bool (*install)(BSG_KSCrash_SentryContext *context);
    void (*uninstall)(void);
} BSG_CrashSentry;

static BSG_CrashSentry bsg_g_sentries[] = {
#if BSG_KSCRASH_HAS_MACH
    {
        BSG_KSCrashTypeMachException, bsg_kscrashsentry_installMachHandler,
        bsg_kscrashsentry_uninstallMachHandler,
    },
#endif
    {
        BSG_KSCrashTypeSignal, bsg_kscrashsentry_installSignalHandler,
        bsg_kscrashsentry_uninstallSignalHandler,
    },
    {
        BSG_KSCrashTypeCPPException,
        bsg_kscrashsentry_installCPPExceptionHandler,
        bsg_kscrashsentry_uninstallCPPExceptionHandler,
    },
    {
        BSG_KSCrashTypeNSException, bsg_kscrashsentry_installNSExceptionHandler,
        bsg_kscrashsentry_uninstallNSExceptionHandler,
    },
    {
        BSG_KSCrashTypeMainThreadDeadlock,
        bsg_kscrashsentry_installDeadlockHandler,
        bsg_kscrashsentry_uninstallDeadlockHandler,
    },
    {
        BSG_KSCrashTypeUserReported,
        bsg_kscrashsentry_installUserExceptionHandler,
        bsg_kscrashsentry_uninstallUserExceptionHandler,
    },
};
static size_t bsg_g_sentriesCount =
    sizeof(bsg_g_sentries) / sizeof(*bsg_g_sentries);

/** Context to fill with crash information. */
static BSG_KSCrash_SentryContext *bsg_g_context = NULL;

/** Keeps track of whether threads have already been suspended or not.
 * This won't handle multiple suspends in a row.
 */
static bool bsg_g_threads_are_running = true;

// ============================================================================
#pragma mark - API -
// ============================================================================

BSG_KSCrashType
bsg_kscrashsentry_installWithContext(BSG_KSCrash_SentryContext *context,
                                     BSG_KSCrashType crashTypes,
                                     void (*onCrash)(void)) {
    if (bsg_ksmachisBeingTraced()) {
        if (context->reportWhenDebuggerIsAttached) {
            BSG_KSLOG_WARN("KSCrash: App is running in a debugger. Crash "
                           "handling is enabled via configuration.");
            BSG_KSLOG_INFO(
                "Installing handlers with context %p, crash types 0x%x.",
                context, crashTypes);
        } else {
            BSG_KSLOG_WARN("KSCrash: App is running in a debugger. Only user "
                           "reported events will be handled.");
            crashTypes = BSG_KSCrashTypeUserReported;
        }
    } else {
        BSG_KSLOG_DEBUG(
            "Installing handlers with context %p, crash types 0x%x.", context,
            crashTypes);
    }

    bsg_g_context = context;
    bsg_kscrashsentry_clearContext(bsg_g_context);
    bsg_g_context->onCrash = onCrash;

    BSG_KSCrashType installed = 0;
    for (size_t i = 0; i < bsg_g_sentriesCount; i++) {
        BSG_CrashSentry *sentry = &bsg_g_sentries[i];
        if (sentry->crashType & crashTypes) {
            if (sentry->install == NULL || sentry->install(context)) {
                installed |= sentry->crashType;
            }
        }
    }

    BSG_KSLOG_DEBUG("Installation complete. Installed types 0x%x.", installed);
    return installed;
}

void bsg_kscrashsentry_uninstall(BSG_KSCrashType crashTypes) {
    BSG_KSLOG_DEBUG("Uninstalling handlers with crash types 0x%x.", crashTypes);
    for (size_t i = 0; i < bsg_g_sentriesCount; i++) {
        BSG_CrashSentry *sentry = &bsg_g_sentries[i];
        if (sentry->crashType & crashTypes) {
            if (sentry->install != NULL) {
                sentry->uninstall();
            }
        }
    }
    BSG_KSLOG_DEBUG("Uninstall complete.");
}

// ============================================================================
#pragma mark - Private API -
// ============================================================================

void bsg_kscrashsentry_suspendThreads(void) {
    BSG_KSLOG_DEBUG("Suspending threads.");
    if (!bsg_g_threads_are_running) {
        BSG_KSLOG_DEBUG("Threads already suspended.");
        return;
    }

    if (bsg_g_context != NULL) {
        int numThreads = sizeof(bsg_g_context->reservedThreads) /
                         sizeof(bsg_g_context->reservedThreads[0]);
        BSG_KSLOG_DEBUG(
            "Suspending all threads except for %d reserved threads.",
            numThreads);
        if (bsg_ksmachsuspendAllThreadsExcept(bsg_g_context->reservedThreads,
                                              numThreads)) {
            BSG_KSLOG_DEBUG("Suspend successful.");
            bsg_g_threads_are_running = false;
        }
    } else {
        BSG_KSLOG_DEBUG("Suspending all threads.");
        if (bsg_ksmachsuspendAllThreads()) {
            BSG_KSLOG_DEBUG("Suspend successful.");
            bsg_g_threads_are_running = false;
        }
    }
    BSG_KSLOG_DEBUG("Suspend complete.");
}

void bsg_kscrashsentry_resumeThreads(void) {
    BSG_KSLOG_DEBUG("Resuming threads.");
    if (bsg_g_threads_are_running) {
        BSG_KSLOG_DEBUG("Threads already resumed.");
        return;
    }

    if (bsg_g_context != NULL) {
        int numThreads = sizeof(bsg_g_context->reservedThreads) /
                         sizeof(bsg_g_context->reservedThreads[0]);
        BSG_KSLOG_DEBUG("Resuming all threads except for %d reserved threads.",
                        numThreads);
        if (bsg_ksmachresumeAllThreadsExcept(bsg_g_context->reservedThreads,
                                             numThreads)) {
            BSG_KSLOG_DEBUG("Resume successful.");
            bsg_g_threads_are_running = true;
        }
    } else {
        BSG_KSLOG_DEBUG("Resuming all threads.");
        if (bsg_ksmachresumeAllThreads()) {
            BSG_KSLOG_DEBUG("Resume successful.");
            bsg_g_threads_are_running = true;
        }
    }
    BSG_KSLOG_DEBUG("Resume complete.");
}

void bsg_kscrashsentry_clearContext(BSG_KSCrash_SentryContext *context) {
    void (*onCrash)(void) = context->onCrash;
    bool threadTracingEnabled = context->threadTracingEnabled;
    bool reportWhenDebuggerIsAttached = context->reportWhenDebuggerIsAttached;
    bool suspendThreadsForUserReported = context->suspendThreadsForUserReported;
    bool writeBinaryImagesForUserReported =
        context->writeBinaryImagesForUserReported;

    memset(context, 0, sizeof(*context));
    context->onCrash = onCrash;

    context->threadTracingEnabled = threadTracingEnabled;
    context->reportWhenDebuggerIsAttached = reportWhenDebuggerIsAttached;
    context->suspendThreadsForUserReported = suspendThreadsForUserReported;
    context->writeBinaryImagesForUserReported =
        writeBinaryImagesForUserReported;
}

void bsg_kscrashsentry_beginHandlingCrash(BSG_KSCrash_SentryContext *context) {
    bsg_kscrashsentry_clearContext(context);
    context->handlingCrash = true;
}
