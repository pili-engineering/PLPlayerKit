//
//  BSG_KSCrashSentry_CPPException.c
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

#import <Foundation/Foundation.h>

#include "BSG_KSCrashSentry_CPPException.h"
#include "BSG_KSCrashSentry_Private.h"
#include "BSG_KSMach.h"

//#define BSG_KSLogger_LocalLevel TRACE
#include "BSG_KSLogger.h"

#include <cxxabi.h>
#include <dlfcn.h>
#include <exception>
#include <execinfo.h>
#include <typeinfo>

#define STACKTRACE_BUFFER_LENGTH 30
#define DESCRIPTION_BUFFER_LENGTH 1000

// Compiler hints for "if" statements
#define likely_if(x) if (__builtin_expect(x, 1))
#define unlikely_if(x) if (__builtin_expect(x, 0))

#ifdef __cplusplus
extern "C" {
#endif
// Internal NSException recorder
void bsg_recordException(NSException *exception);
#ifdef __cplusplus
}
#endif

// ============================================================================
#pragma mark - Globals -
// ============================================================================

/** True if this handler has been installed. */
static volatile sig_atomic_t bsg_g_installed = 0;

/** True if the handler should capture the next stack trace. */
static bool bsg_g_captureNextStackTrace = false;

static std::terminate_handler bsg_g_originalTerminateHandler;

/** Buffer for the backtrace of the most recent exception. */
static uintptr_t bsg_g_stackTrace[STACKTRACE_BUFFER_LENGTH];

/** Number of backtrace entries in the most recent exception. */
static int bsg_g_stackTraceCount = 0;

/** Context to fill with crash information. */
static BSG_KSCrash_SentryContext *bsg_g_context;

// ============================================================================
#pragma mark - Callbacks -
// ============================================================================

typedef void (*cxa_throw_type)(void *, std::type_info *, void (*)(void *));

extern "C" {
void __cxa_throw(void *thrown_exception, std::type_info *tinfo,
                 void (*dest)(void *)) __attribute__((weak));

void __cxa_throw(void *thrown_exception, std::type_info *tinfo,
                 void (*dest)(void *)) {
    if (bsg_g_captureNextStackTrace) {
        bsg_g_stackTraceCount =
            backtrace((void **)bsg_g_stackTrace,
                      sizeof(bsg_g_stackTrace) / sizeof(*bsg_g_stackTrace));
    }

    static cxa_throw_type orig_cxa_throw = NULL;
    unlikely_if(orig_cxa_throw == NULL) {
        orig_cxa_throw = (cxa_throw_type)dlsym(RTLD_NEXT, "__cxa_throw");
    }
    orig_cxa_throw(thrown_exception, tinfo, dest);
    __builtin_unreachable();
}
}

static void CPPExceptionTerminate(void) {
    BSG_KSLOG_DEBUG(@"Trapped c++ exception");

    bool isNSException = false;
    char descriptionBuff[DESCRIPTION_BUFFER_LENGTH];
    const char *name = NULL;
    const char *description = NULL;

    BSG_KSLOG_DEBUG(@"Get exception type name.");
    std::type_info *tinfo = __cxxabiv1::__cxa_current_exception_type();
    if (tinfo != NULL) {
        name = tinfo->name();
    }

    description = descriptionBuff;
    descriptionBuff[0] = 0;

    BSG_KSLOG_DEBUG(@"Discovering what kind of exception was thrown.");
    bsg_g_captureNextStackTrace = false;
    try {
        throw;
    } catch (NSException *exception) {
        BSG_KSLOG_DEBUG(@"Detected NSException. Recording details "
                        @"and letting the current "
                        @"NSException handler deal with it.");
        isNSException = true;
        bsg_recordException(exception);
    } catch (std::exception &exc) {
        strncpy(descriptionBuff, exc.what(), sizeof(descriptionBuff));
    }
#define CATCH_VALUE(TYPE, PRINTFTYPE)                                          \
    catch (TYPE value) {                                                       \
        snprintf(descriptionBuff, sizeof(descriptionBuff), "%" #PRINTFTYPE,    \
                 value);                                                       \
    }
    CATCH_VALUE(char, d)
    CATCH_VALUE(short, d)
    CATCH_VALUE(int, d)
    CATCH_VALUE(long, ld)
    CATCH_VALUE(long long, lld)
    CATCH_VALUE(unsigned char, u)
    CATCH_VALUE(unsigned short, u)
    CATCH_VALUE(unsigned int, u)
    CATCH_VALUE(unsigned long, lu)
    CATCH_VALUE(unsigned long long, llu)
    CATCH_VALUE(float, f)
    CATCH_VALUE(double, f)
    CATCH_VALUE(long double, Lf)
    CATCH_VALUE(char *, s)
    catch (...) {
        description = NULL;
    }
    bsg_g_captureNextStackTrace = (bsg_g_installed != 0);

    if (!isNSException) {
        bool wasHandlingCrash = bsg_g_context->handlingCrash;
        bsg_kscrashsentry_beginHandlingCrash(bsg_g_context);

        if (wasHandlingCrash) {
            BSG_KSLOG_INFO(@"Detected crash in the crash reporter. Restoring "
                           @"original handlers.");
            bsg_g_context->crashedDuringCrashHandling = true;
            bsg_kscrashsentry_uninstall((BSG_KSCrashType)BSG_KSCrashTypeAll);
        }

        BSG_KSLOG_DEBUG(@"Suspending all threads.");
        bsg_kscrashsentry_suspendThreads();

        bsg_g_context->crashType = BSG_KSCrashTypeCPPException;
        bsg_g_context->offendingThread = bsg_ksmachthread_self();
        bsg_g_context->registersAreValid = false;
        bsg_g_context->stackTrace =
            bsg_g_stackTrace + 1; // Don't record __cxa_throw stack entry
        bsg_g_context->stackTraceLength = bsg_g_stackTraceCount - 1;
        bsg_g_context->CPPException.name = name;
        bsg_g_context->crashReason = description;

        BSG_KSLOG_DEBUG(@"Calling main crash handler.");
        bsg_g_context->onCrash();

        BSG_KSLOG_DEBUG(
            @"Crash handling complete. Restoring original handlers.");
        bsg_kscrashsentry_uninstall((BSG_KSCrashType)BSG_KSCrashTypeAll);
        bsg_kscrashsentry_resumeThreads();
    }

    if (bsg_g_originalTerminateHandler != NULL) {
        bsg_g_originalTerminateHandler();
    }
}

// ============================================================================
#pragma mark - Public API -
// ============================================================================

extern "C" bool bsg_kscrashsentry_installCPPExceptionHandler(
    BSG_KSCrash_SentryContext *context) {
    BSG_KSLOG_DEBUG(@"Installing C++ exception handler.");

    if (bsg_g_installed) {
        BSG_KSLOG_DEBUG(@"C++ exception handler already installed.");
        return true;
    }
    bsg_g_installed = 1;

    bsg_g_context = context;

    bsg_g_originalTerminateHandler = std::set_terminate(CPPExceptionTerminate);
    bsg_g_captureNextStackTrace = true;
    return true;
}

extern "C" void bsg_kscrashsentry_uninstallCPPExceptionHandler(void) {
    BSG_KSLOG_DEBUG(@"Uninstalling C++ exception handler.");
    if (!bsg_g_installed) {
        BSG_KSLOG_DEBUG(@"C++ exception handler already uninstalled.");
        return;
    }

    bsg_g_captureNextStackTrace = false;
    std::set_terminate(bsg_g_originalTerminateHandler);
    bsg_g_installed = 0;
}
