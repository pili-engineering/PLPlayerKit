//
//  BSG_KSCrashSentry_User.c
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

#include "BSG_KSCrashSentry_User.h"
#include "BSG_KSCrashSentry_Private.h"
#include "BSG_KSMach.h"

//#define BSG_KSLogger_LocalLevel TRACE
#include "BSG_KSLogger.h"

#include <execinfo.h>
#include <stdlib.h>

/** Context to fill with crash information. */
static BSG_KSCrash_SentryContext *bsg_g_context;

bool bsg_kscrashsentry_installUserExceptionHandler(
    BSG_KSCrash_SentryContext *const context) {
    BSG_KSLOG_DEBUG("Installing user exception handler.");
    bsg_g_context = context;
    return true;
}

void bsg_kscrashsentry_uninstallUserExceptionHandler(void) {
    BSG_KSLOG_DEBUG("Uninstalling user exception handler.");
    bsg_g_context = NULL;
}

void bsg_kscrashsentry_reportUserException(const char *name, const char *reason,
                                           const char *handledState,
                                           const char *overrides,
                                           const char *metadata,
                                           const char *appState,
                                           const char *config,
                                           int discardDepth,
                                           bool terminateProgram) {
    if (bsg_g_context == NULL) {
        BSG_KSLOG_WARN("User-reported exception sentry is not installed. "
                       "Exception has not been recorded.");
    } else {
        bsg_kscrashsentry_beginHandlingCrash(bsg_g_context);

        if (bsg_g_context->suspendThreadsForUserReported) {
            BSG_KSLOG_DEBUG("Suspending all threads");
            bsg_kscrashsentry_suspendThreads();
        }

        BSG_KSLOG_DEBUG("Fetching call stack.");
        int callstackCount = 100;
        uintptr_t callstack[callstackCount];
        callstackCount = backtrace((void **)callstack, callstackCount);
        if (callstackCount <= 0) {
            BSG_KSLOG_ERROR("backtrace() returned call stack length of %d",
                            callstackCount);
            callstackCount = 0;
        }

        BSG_KSLOG_DEBUG("Filling out context.");
        bsg_g_context->crashType = BSG_KSCrashTypeUserReported;
        bsg_g_context->offendingThread = bsg_ksmachthread_self();
        bsg_g_context->registersAreValid = false;
        bsg_g_context->crashReason = reason;
        bsg_g_context->stackTrace = callstack;
        bsg_g_context->stackTraceLength = callstackCount;
        bsg_g_context->userException.name = name;
        bsg_g_context->userException.handledState = handledState;
        bsg_g_context->userException.overrides = overrides;
        bsg_g_context->userException.config = config;
        bsg_g_context->userException.discardDepth = discardDepth;
        bsg_g_context->userException.metadata = metadata;
        bsg_g_context->userException.state = appState;

        BSG_KSLOG_DEBUG("Calling main crash handler.");
        bsg_g_context->onCrash();

        if (terminateProgram) {
            bsg_kscrashsentry_uninstall(BSG_KSCrashTypeAll);
            bsg_kscrashsentry_resumeThreads();
            abort();
        } else {
            bsg_kscrashsentry_clearContext(bsg_g_context);
            bsg_kscrashsentry_resumeThreads();
        }
    }
}
