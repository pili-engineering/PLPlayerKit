//
//  BSG_KSCrashSentry_Signal.c
//
//  Created by Karl Stenerud on 2012-01-28.
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

#include <TargetConditionals.h>

#include "BSG_KSCrashSentry_Private.h"
#include "BSG_KSCrashSentry_Signal.h"

#include "BSG_KSMach.h"
#include "BSG_KSSignalInfo.h"

//#define BSG_KSLogger_LocalLevel TRACE
#include "BSG_KSLogger.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
#pragma mark - Globals -
// ============================================================================

/** Flag noting if we've installed our custom handlers or not.
 * It's not fully thread safe, but it's safer than locking and slightly better
 * than nothing.
 */
static volatile sig_atomic_t bsg_g_installed = 0;

#if !TARGET_OS_TV
/** Our custom signal stack. The signal handler will use this as its stack. */
static stack_t bsg_g_signalStack = {0};
#endif

/** Signal handlers that were installed before we installed ours. */
static struct sigaction *bsg_g_previousSignalHandlers = NULL;

/** Context to fill with crash information. */
static BSG_KSCrash_SentryContext *bsg_g_context;

// ============================================================================
#pragma mark - Callbacks -
// ============================================================================

// Avoiding static functions due to linker issues.

/** Our custom signal handler.
 * Restore the default signal handlers, record the signal information, and
 * write a crash report.
 * Once we're done, re-raise the signal and let the default handlers deal with
 * it.
 *
 * @param sigNum The signal that was raised.
 *
 * @param signalInfo Information about the signal.
 *
 * @param userContext Other contextual information.
 */
void bsg_kssighndl_i_handleSignal(int sigNum, siginfo_t *signalInfo,
                                  void *userContext) {
    BSG_KSLOG_DEBUG("Trapped signal %d", sigNum);
    if (bsg_g_installed) {
        bool wasHandlingCrash = bsg_g_context->handlingCrash;
        bsg_kscrashsentry_beginHandlingCrash(bsg_g_context);

        BSG_KSLOG_DEBUG(
            "Signal handler is installed. Continuing signal handling.");

        BSG_KSLOG_DEBUG("Suspending all threads.");
        bsg_kscrashsentry_suspendThreads();

        if (wasHandlingCrash) {
            BSG_KSLOG_INFO("Detected crash in the crash reporter. Restoring "
                           "original handlers.");
            bsg_g_context->crashedDuringCrashHandling = true;
            bsg_kscrashsentry_uninstall(BSG_KSCrashTypeAsyncSafe);
        }

        BSG_KSLOG_DEBUG("Filling out context.");
        bsg_g_context->crashType = BSG_KSCrashTypeSignal;
        bsg_g_context->offendingThread = bsg_ksmachthread_self();
        bsg_g_context->registersAreValid = true;
        bsg_g_context->faultAddress = (uintptr_t)signalInfo->si_addr;
        bsg_g_context->signal.userContext = userContext;
        bsg_g_context->signal.signalInfo = signalInfo;

        BSG_KSLOG_DEBUG("Calling main crash handler.");
        bsg_g_context->onCrash();

        BSG_KSLOG_DEBUG(
            "Crash handling complete. Restoring original handlers.");
        bsg_kscrashsentry_uninstall(BSG_KSCrashTypeAsyncSafe);
        bsg_kscrashsentry_resumeThreads();
    }

    BSG_KSLOG_DEBUG("Re-raising signal for regular handlers to catch.");
    // This is technically not allowed, but it works in OSX and iOS.
    raise(sigNum);
}

// ============================================================================
#pragma mark - API -
// ============================================================================

bool bsg_kscrashsentry_installSignalHandler(
    BSG_KSCrash_SentryContext *context) {
    BSG_KSLOG_DEBUG("Installing signal handler.");

    if (bsg_g_installed) {
        BSG_KSLOG_DEBUG("Signal handler already installed.");
        return true;
    }
    bsg_g_installed = 1;

    bsg_g_context = context;

#if !TARGET_OS_TV
    if (bsg_g_signalStack.ss_size == 0) {
        BSG_KSLOG_DEBUG("Allocating signal stack area.");
        bsg_g_signalStack.ss_size = SIGSTKSZ;
        bsg_g_signalStack.ss_sp = malloc(bsg_g_signalStack.ss_size);
    }

    BSG_KSLOG_DEBUG("Setting signal stack area.");
    if (sigaltstack(&bsg_g_signalStack, NULL) != 0) {
        BSG_KSLOG_ERROR("signalstack: %s", strerror(errno));
        goto failed;
    }
#endif

    const int *fatalSignals = bsg_kssignal_fatalSignals();
    int fatalSignalsCount = bsg_kssignal_numFatalSignals();

    if (bsg_g_previousSignalHandlers == NULL) {
        BSG_KSLOG_DEBUG("Allocating memory to store previous signal handlers.");
        bsg_g_previousSignalHandlers =
            malloc(sizeof(*bsg_g_previousSignalHandlers) *
                   (unsigned)fatalSignalsCount);
    }

    struct sigaction action = {{0}};
    action.sa_flags = SA_SIGINFO | SA_ONSTACK;
#ifdef __LP64__
    action.sa_flags |= SA_64REGSET;
#endif
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = &bsg_kssighndl_i_handleSignal;

    for (int i = 0; i < fatalSignalsCount; i++) {
        BSG_KSLOG_DEBUG("Assigning handler for signal %d", fatalSignals[i]);
        if (sigaction(fatalSignals[i], &action,
                      &bsg_g_previousSignalHandlers[i]) != 0) {
            char sigNameBuff[30];
            const char *sigName = bsg_kssignal_signalName(fatalSignals[i]);
            if (sigName == NULL) {
                snprintf(sigNameBuff, sizeof(sigNameBuff), "%d",
                         fatalSignals[i]);
                sigName = sigNameBuff;
            }
            BSG_KSLOG_ERROR("sigaction (%s): %s", sigName, strerror(errno));
            // Try to reverse the damage
            for (i--; i >= 0; i--) {
                sigaction(fatalSignals[i], &bsg_g_previousSignalHandlers[i],
                          NULL);
            }
            goto failed;
        }
    }
    BSG_KSLOG_DEBUG("Signal handlers installed.");
    return true;

failed:
    BSG_KSLOG_DEBUG("Failed to install signal handlers.");
    bsg_g_installed = 0;
    return false;
}

void bsg_kscrashsentry_uninstallSignalHandler(void) {
    BSG_KSLOG_DEBUG("Uninstalling signal handlers.");
    if (!bsg_g_installed) {
        BSG_KSLOG_DEBUG("Signal handlers were already uninstalled.");
        return;
    }

    const int *fatalSignals = bsg_kssignal_fatalSignals();
    int fatalSignalsCount = bsg_kssignal_numFatalSignals();

    for (int i = 0; i < fatalSignalsCount; i++) {
        BSG_KSLOG_DEBUG("Restoring original handler for signal %d",
                        fatalSignals[i]);
        sigaction(fatalSignals[i], &bsg_g_previousSignalHandlers[i], NULL);
    }

    BSG_KSLOG_DEBUG("Signal handlers uninstalled.");
    bsg_g_installed = 0;
}
