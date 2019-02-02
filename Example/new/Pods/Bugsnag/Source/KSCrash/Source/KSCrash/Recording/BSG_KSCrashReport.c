//
//  BSG_KSCrashReport.m
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

#include "BSG_KSCrashReport.h"

#include "BSG_KSBacktrace_Private.h"
#include "BSG_KSCrashReportFields.h"
#include "BSG_KSCrashReportVersion.h"
#include "BSG_KSDynamicLinker.h"
#include "BSG_KSFileUtils.h"
#include "BSG_KSJSONCodec.h"
#include "BSG_KSMach.h"
#include "BSG_KSObjC.h"
#include "BSG_KSSignalInfo.h"
#include "BSG_KSString.h"
#include "BSG_KSZombie.h"

//#define BSG_kSLogger_LocalLevel TRACE
#include "BSG_KSLogger.h"
#include "BSG_KSCrashContext.h"

#ifdef __arm64__
#include <sys/_types/_ucontext64.h>
#define BSG_UC_MCONTEXT uc_mcontext64
typedef ucontext64_t SignalUserContext;
#else
#define BSG_UC_MCONTEXT uc_mcontext
typedef ucontext_t SignalUserContext;
#endif

// Note: Avoiding static functions due to linker issues.

// ============================================================================
#pragma mark - Constants -
// ============================================================================

/** Maximum depth allowed for a backtrace. */
#define BSG_kMaxBacktraceDepth 150

/** Default number of objects, subobjects, and ivars to record from a memory loc
 */
#define BSG_kDefaultMemorySearchDepth 15

/** Length at which we consider a backtrace to represent a stack overflow.
 * If it reaches this point, we start cutting off from the top of the stack
 * rather than the bottom.
 */
#define BSG_kStackOverflowThreshold 200

/** Maximum number of lines to print when printing a stack trace to the console.
 */
#define BSG_kMaxStackTracePrintLines 40

/** How far to search the stack (in pointer sized jumps) for notable data. */
#define BSG_kStackNotableSearchBackDistance 20
#define BSG_kStackNotableSearchForwardDistance 10

/** How much of the stack to dump (in pointer sized jumps). */
#define BSG_kStackContentsPushedDistance 20
#define BSG_kStackContentsPoppedDistance 10
#define BSG_kStackContentsTotalDistance                                        \
    (BSG_kStackContentsPushedDistance + BSG_kStackContentsPoppedDistance)

/** The minimum length for a valid string. */
#define BSG_kMinStringLength 4

// ============================================================================
#pragma mark - Formatting -
// ============================================================================

#if defined(__LP64__)
#define BSG_TRACE_FMT "%-4d%-31s 0x%016lx %s + %lu"
#define BSG_POINTER_FMT "0x%016lx"
#define BSG_POINTER_SHORT_FMT "0x%lx"
#else
#define BSG_TRACE_FMT "%-4d%-31s 0x%08lx %s + %lu"
#define BSG_POINTER_FMT "0x%08lx"
#define BSG_POINTER_SHORT_FMT "0x%lx"
#endif

// ============================================================================
#pragma mark - JSON Encoding -
// ============================================================================

#define bsg_getJsonContext(REPORT_WRITER)                                      \
    ((BSG_KSJSONEncodeContext *)((REPORT_WRITER)->context))

/** Used for writing hex string values. */
static const char bsg_g_hexNybbles[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

// ============================================================================
#pragma mark - Runtime Config -
// ============================================================================

static BSG_KSCrash_IntrospectionRules *bsg_g_introspectionRules;

#pragma mark Callbacks

void bsg_kscrw_i_addBooleanElement(const BSG_KSCrashReportWriter *const writer,
                                   const char *const key, const bool value) {
    bsg_ksjsonaddBooleanElement(bsg_getJsonContext(writer), key, value);
}

void bsg_kscrw_i_addFloatingPointElement(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const double value) {
    bsg_ksjsonaddFloatingPointElement(bsg_getJsonContext(writer), key, value);
}

void bsg_kscrw_i_addIntegerElement(const BSG_KSCrashReportWriter *const writer,
                                   const char *const key,
                                   const long long value) {
    bsg_ksjsonaddIntegerElement(bsg_getJsonContext(writer), key, value);
}

void bsg_kscrw_i_addUIntegerElement(const BSG_KSCrashReportWriter *const writer,
                                    const char *const key,
                                    const unsigned long long value) {
    bsg_ksjsonaddIntegerElement(bsg_getJsonContext(writer), key,
                                (long long)value);
}

void bsg_kscrw_i_addStringElement(const BSG_KSCrashReportWriter *const writer,
                                  const char *const key,
                                  const char *const value) {
    bsg_ksjsonaddStringElement(bsg_getJsonContext(writer), key, value,
                               BSG_KSJSON_SIZE_AUTOMATIC);
}

void bsg_kscrw_i_addTextFileElement(const BSG_KSCrashReportWriter *const writer,
                                    const char *const key,
                                    const char *const filePath) {
    const int fd = open(filePath, O_RDONLY);
    if (fd < 0) {
        BSG_KSLOG_ERROR("Could not open file %s: %s", filePath,
                        strerror(errno));
        return;
    }

    if (bsg_ksjsonbeginStringElement(bsg_getJsonContext(writer), key) !=
        BSG_KSJSON_OK) {
        BSG_KSLOG_ERROR("Could not start string element");
        goto done;
    }

    char buffer[512];
    ssize_t bytesRead;
    for (bytesRead = read(fd, buffer, sizeof(buffer)); bytesRead > 0;
         bytesRead = read(fd, buffer, sizeof(buffer))) {
        if (bsg_ksjsonappendStringElement(bsg_getJsonContext(writer), buffer,
                                          (size_t)bytesRead) != BSG_KSJSON_OK) {
            BSG_KSLOG_ERROR("Could not append string element");
            goto done;
        }
    }

done:
    bsg_ksjsonendStringElement(bsg_getJsonContext(writer));
    close(fd);
}

void bsg_kscrw_i_addDataElement(const BSG_KSCrashReportWriter *const writer,
                                const char *const key, const char *const value,
                                const size_t length) {
    bsg_ksjsonaddDataElement(bsg_getJsonContext(writer), key, value, length);
}

void bsg_kscrw_i_beginDataElement(const BSG_KSCrashReportWriter *const writer,
                                  const char *const key) {
    bsg_ksjsonbeginDataElement(bsg_getJsonContext(writer), key);
}

void bsg_kscrw_i_appendDataElement(const BSG_KSCrashReportWriter *const writer,
                                   const char *const value,
                                   const size_t length) {
    bsg_ksjsonappendDataElement(bsg_getJsonContext(writer), value, length);
}

void bsg_kscrw_i_endDataElement(const BSG_KSCrashReportWriter *const writer) {
    bsg_ksjsonendDataElement(bsg_getJsonContext(writer));
}

void bsg_kscrw_i_addUUIDElement(const BSG_KSCrashReportWriter *const writer,
                                const char *const key,
                                const unsigned char *const value) {
    if (value == NULL) {
        bsg_ksjsonaddNullElement(bsg_getJsonContext(writer), key);
    } else {
        char uuidBuffer[37];
        const unsigned char *src = value;
        char *dst = uuidBuffer;
        for (int i = 0; i < 4; i++) {
            *dst++ = bsg_g_hexNybbles[(*src >> 4) & 15];
            *dst++ = bsg_g_hexNybbles[(*src++) & 15];
        }
        *dst++ = '-';
        for (int i = 0; i < 2; i++) {
            *dst++ = bsg_g_hexNybbles[(*src >> 4) & 15];
            *dst++ = bsg_g_hexNybbles[(*src++) & 15];
        }
        *dst++ = '-';
        for (int i = 0; i < 2; i++) {
            *dst++ = bsg_g_hexNybbles[(*src >> 4) & 15];
            *dst++ = bsg_g_hexNybbles[(*src++) & 15];
        }
        *dst++ = '-';
        for (int i = 0; i < 2; i++) {
            *dst++ = bsg_g_hexNybbles[(*src >> 4) & 15];
            *dst++ = bsg_g_hexNybbles[(*src++) & 15];
        }
        *dst++ = '-';
        for (int i = 0; i < 6; i++) {
            *dst++ = bsg_g_hexNybbles[(*src >> 4) & 15];
            *dst++ = bsg_g_hexNybbles[(*src++) & 15];
        }

        bsg_ksjsonaddStringElement(bsg_getJsonContext(writer), key, uuidBuffer,
                                   (size_t)(dst - uuidBuffer));
    }
}

void bsg_kscrw_i_addJSONElement(const BSG_KSCrashReportWriter *const writer,
                                const char *const key,
                                const char *const jsonElement) {
    int jsonResult = bsg_ksjsonaddJSONElement(bsg_getJsonContext(writer), key,
                                              jsonElement, strlen(jsonElement));
    if (jsonResult != BSG_KSJSON_OK) {
        char errorBuff[100];
        snprintf(errorBuff, sizeof(errorBuff), "Invalid JSON data: %s",
                 bsg_ksjsonstringForError(jsonResult));
        bsg_ksjsonbeginObject(bsg_getJsonContext(writer), key);
        bsg_ksjsonaddStringElement(bsg_getJsonContext(writer),
                                   BSG_KSCrashField_Error, errorBuff,
                                   BSG_KSJSON_SIZE_AUTOMATIC);
        bsg_ksjsonaddStringElement(bsg_getJsonContext(writer),
                                   BSG_KSCrashField_JSONData, jsonElement,
                                   BSG_KSJSON_SIZE_AUTOMATIC);
        bsg_ksjsonendContainer(bsg_getJsonContext(writer));
    }
}

void bsg_kscrw_i_addJSONElementFromFile(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const char *const filePath) {
    const int fd = open(filePath, O_RDONLY);
    if (fd < 0) {
        BSG_KSLOG_ERROR("Could not open file %s: %s", filePath,
                        strerror(errno));
        return;
    }

    if (bsg_ksjsonbeginElement(bsg_getJsonContext(writer), key) !=
        BSG_KSJSON_OK) {
        BSG_KSLOG_ERROR("Could not start JSON element");
        goto done;
    }

    char buffer[512];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        if (bsg_ksjsonaddRawJSONData(bsg_getJsonContext(writer), buffer,
                                     (size_t)bytesRead) != BSG_KSJSON_OK) {
            BSG_KSLOG_ERROR("Could not append JSON data");
            goto done;
        }
    }

done:
    close(fd);
}

void bsg_kscrw_i_beginObject(const BSG_KSCrashReportWriter *const writer,
                             const char *const key) {
    bsg_ksjsonbeginObject(bsg_getJsonContext(writer), key);
}

void bsg_kscrw_i_beginArray(const BSG_KSCrashReportWriter *const writer,
                            const char *const key) {
    bsg_ksjsonbeginArray(bsg_getJsonContext(writer), key);
}

void bsg_kscrw_i_endContainer(const BSG_KSCrashReportWriter *const writer) {
    bsg_ksjsonendContainer(bsg_getJsonContext(writer));
}

int bsg_kscrw_i_addJSONData(const char *const data, const size_t length,
                            void *const userData) {
    const int fd = *((int *)userData);
    const bool success = bsg_ksfuwriteBytesToFD(fd, data, (ssize_t)length);
    return success ? BSG_KSJSON_OK : BSG_KSJSON_ERROR_CANNOT_ADD_DATA;
}

// ============================================================================
#pragma mark - Utility -
// ============================================================================

/** Check if a memory address points to a valid null terminated UTF-8 string.
 *
 * @param address The address to check.
 *
 * @return true if the address points to a string.
 */
bool bsg_kscrw_i_isValidString(const void *const address) {
    if ((void *)address == NULL) {
        return false;
    }

    char buffer[500];
    if ((uintptr_t)address + sizeof(buffer) < (uintptr_t)address) {
        // Wrapped around the address range.
        return false;
    }
    if (bsg_ksmachcopyMem(address, buffer, sizeof(buffer)) != KERN_SUCCESS) {
        return false;
    }
    return bsg_ksstring_isNullTerminatedUTF8String(buffer, BSG_kMinStringLength,
                                                   sizeof(buffer));
}

/** Get all parts of the machine state required for a dump.
 * This includes basic thread state, and exception registers.
 *
 * @param thread The thread to get state for.
 *
 * @param machineContextBuffer The machine context to fill out.
 */
bool bsg_kscrw_i_fetchMachineState(
    const thread_t thread, BSG_STRUCT_MCONTEXT_L *const machineContextBuffer) {
    if (!bsg_ksmachthreadState(thread, machineContextBuffer)) {
        return false;
    }

    if (!bsg_ksmachexceptionState(thread, machineContextBuffer)) {
        return false;
    }

    return true;
}

/** Get the machine context for the specified thread.
 *
 * This function will choose how to fetch the machine context based on what kind
 * of thread it is (current, crashed, other), and what kind of crash occured.
 * It may store the context in machineContextBuffer unless it can be fetched
 * directly from memory. Do not count on machineContextBuffer containing
 * anything. Always use the return value.
 *
 * @param crash The crash handler context.
 *
 * @param thread The thread to get a machine context for.
 *
 * @param machineContextBuffer A place to store the context, if needed.
 *
 * @return A pointer to the crash context, or NULL if not found.
 */
BSG_STRUCT_MCONTEXT_L *bsg_kscrw_i_getMachineContext(
    const BSG_KSCrash_SentryContext *const crash, const thread_t thread,
    BSG_STRUCT_MCONTEXT_L *const machineContextBuffer) {
    if (thread == crash->offendingThread) {
        if (crash->crashType == BSG_KSCrashTypeSignal) {
            return ((SignalUserContext *)crash->signal.userContext)
                ->BSG_UC_MCONTEXT;
        }
    }

    if (thread == bsg_ksmachthread_self()) {
        return NULL;
    }

    if (!bsg_kscrw_i_fetchMachineState(thread, machineContextBuffer)) {
        BSG_KSLOG_ERROR("Failed to fetch machine state for thread %d", thread);
        return NULL;
    }

    return machineContextBuffer;
}

/** Get the backtrace for the specified thread.
 *
 * This function will choose how to fetch the backtrace based on machine context
 * availability andwhat kind of crash occurred. It may store the backtrace in
 * backtraceBuffer unless it can be fetched directly from memory. Do not count
 * on backtraceBuffer containing anything. Always use the return value.
 *
 * @param crash The crash handler context.
 *
 * @param thread The thread to get a machine context for.
 *
 * @param machineContext The machine context (can be NULL).
 *
 * @param backtraceBuffer A place to store the backtrace, if needed.
 *
 * @param backtraceLength In: The length of backtraceBuffer.
 *                        Out: The length of the backtrace.
 *
 * @param skippedEntries Out: The number of entries that were skipped due to
 *                             stack overflow.
 *
 * @return The backtrace, or NULL if not found.
 */
uintptr_t *bsg_kscrw_i_getBacktrace(
    const BSG_KSCrash_SentryContext *const crash, const thread_t thread,
    const BSG_STRUCT_MCONTEXT_L *const machineContext,
    uintptr_t *const backtraceBuffer, int *const backtraceLength,
    int *const skippedEntries) {
    if (thread == crash->offendingThread) {
        if (crash->stackTrace != NULL && crash->stackTraceLength > 0 &&
            (crash->crashType &
             (BSG_KSCrashTypeCPPException | BSG_KSCrashTypeNSException |
              BSG_KSCrashTypeUserReported))) {
            *backtraceLength = crash->stackTraceLength;
            return crash->stackTrace;
        }
    }

    if (machineContext == NULL) {
        return NULL;
    }

    int actualSkippedEntries = 0;
    int actualLength = bsg_ksbt_backtraceLength(machineContext);
    if (actualLength >= BSG_kStackOverflowThreshold) {
        actualSkippedEntries = actualLength - *backtraceLength;
    }

    *backtraceLength =
        bsg_ksbt_backtraceThreadState(machineContext, backtraceBuffer,
                                      actualSkippedEntries, *backtraceLength);
    if (skippedEntries != NULL) {
        *skippedEntries = actualSkippedEntries;
    }
    return backtraceBuffer;
}

/** Check if the stack for the specified thread has overflowed.
 *
 * @param crash The crash handler context.
 *
 * @param thread The thread to check.
 *
 * @return true if the thread's stack has overflowed.
 */
bool bsg_kscrw_i_isStackOverflow(const BSG_KSCrash_SentryContext *const crash,
                                 const thread_t thread) {
    BSG_STRUCT_MCONTEXT_L concreteMachineContext;
    BSG_STRUCT_MCONTEXT_L *machineContext =
        bsg_kscrw_i_getMachineContext(crash, thread, &concreteMachineContext);
    if (machineContext == NULL) {
        return false;
    }

    return bsg_ksbt_isBacktraceTooLong(machineContext,
                                       BSG_kStackOverflowThreshold);
}

// ============================================================================
#pragma mark - Console Logging -
// ============================================================================

/** Print the crash type and location to the log.
 *
 * @param sentryContext The crash sentry context.
 */
void bsg_kscrw_i_logCrashType(
    const BSG_KSCrash_SentryContext *const sentryContext) {
    switch (sentryContext->crashType) {
    case BSG_KSCrashTypeMachException: {
        int machExceptionType = sentryContext->mach.type;
        kern_return_t machCode = (kern_return_t)sentryContext->mach.code;
        const char *machExceptionName =
            bsg_ksmachexceptionName(machExceptionType);
        const char *machCodeName =
            machCode == 0 ? NULL : bsg_ksmachkernelReturnCodeName(machCode);
        BSG_KSLOGBASIC_INFO("App crashed due to mach exception: [%s: %s] at %p",
                            machExceptionName, machCodeName,
                            sentryContext->faultAddress);
        break;
    }
    case BSG_KSCrashTypeCPPException: {
        BSG_KSLOG_INFO("App crashed due to C++ exception: %s: %s",
                       sentryContext->CPPException.name,
                       sentryContext->crashReason);
        break;
    }
    case BSG_KSCrashTypeNSException: {
        BSG_KSLOGBASIC_INFO("App crashed due to NSException: %s: %s",
                            sentryContext->NSException.name,
                            sentryContext->crashReason);
        break;
    }
    case BSG_KSCrashTypeSignal: {
        int sigNum = sentryContext->signal.signalInfo->si_signo;
        int sigCode = sentryContext->signal.signalInfo->si_code;
        const char *sigName = bsg_kssignal_signalName(sigNum);
        const char *sigCodeName = bsg_kssignal_signalCodeName(sigNum, sigCode);
        BSG_KSLOGBASIC_INFO("App crashed due to signal: [%s, %s] at %08x",
                            sigName, sigCodeName, sentryContext->faultAddress);
        break;
    }
    case BSG_KSCrashTypeMainThreadDeadlock: {
        BSG_KSLOGBASIC_INFO("Main thread deadlocked");
        break;
    }
    case BSG_KSCrashTypeUserReported: {
        BSG_KSLOG_INFO("App crashed due to user specified exception: %s",
                       sentryContext->crashReason);
        break;
    }
    }
}

/** Print a backtrace entry in the standard format to the log.
 *
 * @param entryNum The backtrace entry number.
 *
 * @param address The program counter value (instruction address).
 *
 * @param dlInfo Information about the nearest symbols to the address.
 */
void bsg_kscrw_i_logBacktraceEntry(const int entryNum, const uintptr_t address,
                                   const Dl_info *const dlInfo) {
    char faddrBuff[20];
    char saddrBuff[20];

    const char *fname = bsg_ksfulastPathEntry(dlInfo->dli_fname);
    if (fname == NULL) {
        sprintf(faddrBuff, BSG_POINTER_FMT, (uintptr_t)dlInfo->dli_fbase);
        fname = faddrBuff;
    }

    uintptr_t offset = address - (uintptr_t)dlInfo->dli_saddr;
    const char *sname = dlInfo->dli_sname;
    if (sname == NULL) {
        sprintf(saddrBuff, BSG_POINTER_SHORT_FMT, (uintptr_t)dlInfo->dli_fbase);
        sname = saddrBuff;
        offset = address - (uintptr_t)dlInfo->dli_fbase;
    }

    BSG_KSLOGBASIC_ALWAYS(BSG_TRACE_FMT, entryNum, fname, address, sname,
                          offset);
}

/** Print a backtrace to the log.
 *
 * @param backtrace The backtrace to print.
 *
 * @param backtraceLength The length of the backtrace.
 */
void bsg_kscrw_i_logBacktrace(const uintptr_t *const backtrace,
                              const int backtraceLength,
                              const int skippedEntries) {
    if (backtraceLength > 0) {
        Dl_info symbolicated[backtraceLength];
        bsg_ksbt_symbolicate(backtrace, symbolicated, backtraceLength,
                             skippedEntries);

        for (int i = 0; i < backtraceLength; i++) {
            bsg_kscrw_i_logBacktraceEntry(i, backtrace[i], &symbolicated[i]);
        }
    }
}

/** Print the backtrace for the crashed thread to the log.
 *
 * @param crash The crash handler context.
 */
void bsg_kscrw_i_logCrashThreadBacktrace(
    const BSG_KSCrash_SentryContext *const crash) {
    thread_t thread = crash->offendingThread;
    BSG_STRUCT_MCONTEXT_L concreteMachineContext;
    uintptr_t concreteBacktrace[BSG_kMaxStackTracePrintLines];
    int backtraceLength =
        sizeof(concreteBacktrace) / sizeof(*concreteBacktrace);

    BSG_STRUCT_MCONTEXT_L *machineContext =
        bsg_kscrw_i_getMachineContext(crash, thread, &concreteMachineContext);

    int skippedEntries = 0;
    uintptr_t *backtrace = bsg_kscrw_i_getBacktrace(
        crash, thread, machineContext, concreteBacktrace, &backtraceLength,
        &skippedEntries);

    if (backtrace != NULL) {
        bsg_kscrw_i_logBacktrace(backtrace, backtraceLength, skippedEntries);
    }
}

// ============================================================================
#pragma mark - Report Writing -
// ============================================================================

/** Write the contents of a memory location.
 * Also writes meta information about the data.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param address The memory address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
void bsg_kscrw_i_writeMemoryContents(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const uintptr_t address, int *limit);

/** Write a string to the report.
 * This will only print the first child of the array.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param objectAddress The object's address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
void bsg_kscrw_i_writeNSStringContents(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const uintptr_t objectAddress, __unused int *limit) {
    const void *object = (const void *)objectAddress;
    char buffer[200];
    if (bsg_ksobjc_copyStringContents(object, buffer, sizeof(buffer))) {
        writer->addStringElement(writer, key, buffer);
    }
}

/** Write a URL to the report.
 * This will only print the first child of the array.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param objectAddress The object's address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
void bsg_kscrw_i_writeURLContents(const BSG_KSCrashReportWriter *const writer,
                                  const char *const key,
                                  const uintptr_t objectAddress,
                                  __unused int *limit) {
    const void *object = (const void *)objectAddress;
    char buffer[200];
    if (bsg_ksobjc_copyStringContents(object, buffer, sizeof(buffer))) {
        writer->addStringElement(writer, key, buffer);
    }
}

/** Write a date to the report.
 * This will only print the first child of the array.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param objectAddress The object's address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
void bsg_kscrw_i_writeDateContents(const BSG_KSCrashReportWriter *const writer,
                                   const char *const key,
                                   const uintptr_t objectAddress,
                                   __unused int *limit) {
    const void *object = (const void *)objectAddress;
    writer->addFloatingPointElement(writer, key,
                                    bsg_ksobjc_dateContents(object));
}

/** Write a number to the report.
 * This will only print the first child of the array.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param objectAddress The object's address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
void bsg_kscrw_i_writeNumberContents(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const uintptr_t objectAddress, __unused int *limit) {
    const void *object = (const void *)objectAddress;
    writer->addFloatingPointElement(writer, key,
                                    bsg_ksobjc_numberAsFloat(object));
}

/** Write an array to the report.
 * This will only print the first child of the array.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param objectAddress The object's address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
void bsg_kscrw_i_writeArrayContents(const BSG_KSCrashReportWriter *const writer,
                                    const char *const key,
                                    const uintptr_t objectAddress, int *limit) {
    const void *object = (const void *)objectAddress;
    uintptr_t firstObject;
    if (bsg_ksobjc_arrayContents(object, &firstObject, 1) == 1) {
        bsg_kscrw_i_writeMemoryContents(writer, key, firstObject, limit);
    }
}

/** Write out ivar information about an unknown object.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param objectAddress The object's address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
void bsg_kscrw_i_writeUnknownObjectContents(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const uintptr_t objectAddress, int *limit) {
    (*limit)--;
    const void *object = (const void *)objectAddress;
    BSG_KSObjCIvar ivars[10];
    char s8;
    short s16;
    int sInt;
    long s32;
    long long s64;
    unsigned char u8;
    unsigned short u16;
    unsigned int uInt;
    unsigned long u32;
    unsigned long long u64;
    float f32;
    double f64;
    _Bool b;
    void *pointer;

    writer->beginObject(writer, key);
    {
        if (bsg_ksobjc_bsg_isTaggedPointer(object)) {
            writer->addIntegerElement(
                writer, "tagged_payload",
                (long long)bsg_ksobjc_taggedPointerPayload(object));
        } else {
            const void *class = bsg_ksobjc_isaPointer(object);
            size_t ivarCount = bsg_ksobjc_ivarList(
                class, ivars, sizeof(ivars) / sizeof(*ivars));
            *limit -= (int)ivarCount;
            for (size_t i = 0; i < ivarCount; i++) {
                BSG_KSObjCIvar *ivar = &ivars[i];
                
                if (ivar->type == NULL) {
                    BSG_KSLOG_ERROR("Found null ivar :(");
                    continue;
                }
                
                switch (ivar->type[0]) {
                case 'c':
                    bsg_ksobjc_ivarValue(object, ivar->index, &s8);
                    writer->addIntegerElement(writer, ivar->name, s8);
                    break;
                case 'i':
                    bsg_ksobjc_ivarValue(object, ivar->index, &sInt);
                    writer->addIntegerElement(writer, ivar->name, sInt);
                    break;
                case 's':
                    bsg_ksobjc_ivarValue(object, ivar->index, &s16);
                    writer->addIntegerElement(writer, ivar->name, s16);
                    break;
                case 'l':
                    bsg_ksobjc_ivarValue(object, ivar->index, &s32);
                    writer->addIntegerElement(writer, ivar->name, s32);
                    break;
                case 'q':
                    bsg_ksobjc_ivarValue(object, ivar->index, &s64);
                    writer->addIntegerElement(writer, ivar->name, s64);
                    break;
                case 'C':
                    bsg_ksobjc_ivarValue(object, ivar->index, &u8);
                    writer->addUIntegerElement(writer, ivar->name, u8);
                    break;
                case 'I':
                    bsg_ksobjc_ivarValue(object, ivar->index, &uInt);
                    writer->addUIntegerElement(writer, ivar->name, uInt);
                    break;
                case 'S':
                    bsg_ksobjc_ivarValue(object, ivar->index, &u16);
                    writer->addUIntegerElement(writer, ivar->name, u16);
                    break;
                case 'L':
                    bsg_ksobjc_ivarValue(object, ivar->index, &u32);
                    writer->addUIntegerElement(writer, ivar->name, u32);
                    break;
                case 'Q':
                    bsg_ksobjc_ivarValue(object, ivar->index, &u64);
                    writer->addUIntegerElement(writer, ivar->name, u64);
                    break;
                case 'f':
                    bsg_ksobjc_ivarValue(object, ivar->index, &f32);
                    writer->addFloatingPointElement(writer, ivar->name, f32);
                    break;
                case 'd':
                    bsg_ksobjc_ivarValue(object, ivar->index, &f64);
                    writer->addFloatingPointElement(writer, ivar->name, f64);
                    break;
                case 'B':
                    bsg_ksobjc_ivarValue(object, ivar->index, &b);
                    writer->addBooleanElement(writer, ivar->name, b);
                    break;
                case '*':
                case '@':
                case '#':
                case ':':
                    bsg_ksobjc_ivarValue(object, ivar->index, &pointer);
                    bsg_kscrw_i_writeMemoryContents(writer, ivar->name,
                                                    (uintptr_t)pointer, limit);
                    break;
                default:
                    BSG_KSLOG_DEBUG("%s: Unknown ivar type [%s]", ivar->name,
                                    ivar->type);
                }
            }
        }
    }
    writer->endContainer(writer);
}

bool bsg_kscrw_i_isRestrictedClass(const char *name) {
    if (bsg_g_introspectionRules->restrictedClasses != NULL) {
        for (size_t i = 0; i < bsg_g_introspectionRules->restrictedClassesCount;
             i++) {
            if (strcmp(name, bsg_g_introspectionRules->restrictedClasses[i]) ==
                0) {
                return true;
            }
        }
    }
    return false;
}

/** Write the contents of a memory location.
 * Also writes meta information about the data.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param address The memory address.
 *
 * @param limit How many more subreferenced objects to write, if any.
 */
void bsg_kscrw_i_writeMemoryContents(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const uintptr_t address, int *limit) {
    (*limit)--;
    const void *object = (const void *)address;
    writer->beginObject(writer, key);
    {
        writer->addUIntegerElement(writer, BSG_KSCrashField_Address, address);
        const char *zombieClassName = bsg_kszombie_className(object);
        if (zombieClassName != NULL) {
            writer->addStringElement(writer, BSG_KSCrashField_LastDeallocObject,
                                     zombieClassName);
        }
        switch (bsg_ksobjc_objectType(object)) {
        case BSG_KSObjCTypeUnknown:
            if (object == NULL) {
                writer->addStringElement(writer, BSG_KSCrashField_Type,
                                         BSG_KSCrashMemType_NullPointer);
            } else if (bsg_kscrw_i_isValidString(object)) {
                writer->addStringElement(writer, BSG_KSCrashField_Type,
                                         BSG_KSCrashMemType_String);
                writer->addStringElement(writer, BSG_KSCrashField_Value,
                                         (const char *)object);
            } else {
                writer->addStringElement(writer, BSG_KSCrashField_Type,
                                         BSG_KSCrashMemType_Unknown);
            }
            break;
        case BSG_KSObjCTypeClass:
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashMemType_Class);
            writer->addStringElement(writer, BSG_KSCrashField_Class,
                                     bsg_ksobjc_className(object));
            break;
        case BSG_KSObjCTypeObject: {
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashMemType_Object);
            const char *className = bsg_ksobjc_objectClassName(object);
            writer->addStringElement(writer, BSG_KSCrashField_Class, className);
            if (!bsg_kscrw_i_isRestrictedClass(className)) {
                switch (bsg_ksobjc_objectClassType(object)) {
                case BSG_KSObjCClassTypeString:
                    bsg_kscrw_i_writeNSStringContents(
                        writer, BSG_KSCrashField_Value, address, limit);
                    break;
                case BSG_KSObjCClassTypeURL:
                    bsg_kscrw_i_writeURLContents(writer, BSG_KSCrashField_Value,
                                                 address, limit);
                    break;
                case BSG_KSObjCClassTypeDate:
                    bsg_kscrw_i_writeDateContents(
                        writer, BSG_KSCrashField_Value, address, limit);
                    break;
                case BSG_KSObjCClassTypeArray:
                    if (*limit > 0) {
                        bsg_kscrw_i_writeArrayContents(
                            writer, BSG_KSCrashField_FirstObject, address,
                            limit);
                    }
                    break;
                case BSG_KSObjCClassTypeNumber:
                    bsg_kscrw_i_writeNumberContents(
                        writer, BSG_KSCrashField_Value, address, limit);
                    break;
                case BSG_KSObjCClassTypeDictionary:
                case BSG_KSObjCClassTypeException:
                    // TODO: Implement these.
                    if (*limit > 0) {
                        bsg_kscrw_i_writeUnknownObjectContents(
                            writer, BSG_KSCrashField_Ivars, address, limit);
                    }
                    break;
                case BSG_KSObjCClassTypeUnknown:
                    if (*limit > 0) {
                        bsg_kscrw_i_writeUnknownObjectContents(
                            writer, BSG_KSCrashField_Ivars, address, limit);
                    }
                    break;
                }
            }
            break;
        }
        case BSG_KSObjCTypeBlock:
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashMemType_Block);
            const char *className = bsg_ksobjc_objectClassName(object);
            writer->addStringElement(writer, BSG_KSCrashField_Class, className);
            break;
        }
    }
    writer->endContainer(writer);
}

bool bsg_kscrw_i_isValidPointer(const uintptr_t address) {
    if (address == (uintptr_t)NULL) {
        return false;
    }

    if (bsg_ksobjc_bsg_isTaggedPointer((const void *)address)) {
        if (!bsg_ksobjc_isValidTaggedPointer((const void *)address)) {
            return false;
        }
    }

    return true;
}

/**
 * Strip higher order bits from addresses which aren't related to the actual
 * location.
 */
#define BSG_ValidPointerMask  0x0000000fffffffff

/** Write the contents of a memory location only if it contains notable data.
 * Also writes meta information about the data.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param rawAddress The memory address.
 */
void bsg_kscrw_i_writeMemoryContentsIfNotable(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const uintptr_t rawAddress) {
    uintptr_t address = rawAddress;
    if (!bsg_kscrw_i_isValidPointer(address)) {
        address &= BSG_ValidPointerMask;
        if (!bsg_kscrw_i_isValidPointer(address)) {
            return;
        }
    }

    const void *object = (const void *)address;

    if (bsg_ksobjc_objectType(object) == BSG_KSObjCTypeUnknown &&
        bsg_kszombie_className(object) == NULL &&
        !bsg_kscrw_i_isValidString(object)) {
        // Nothing notable about this memory location.
        return;
    }

    int limit = BSG_kDefaultMemorySearchDepth;
    bsg_kscrw_i_writeMemoryContents(writer, key, address, &limit);
}

/** Look for a hex value in a string and try to write whatever it references.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param string The string to search.
 */
void bsg_kscrw_i_writeAddressReferencedByString(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const char *string) {
    uint64_t address = 0;
    if (string == NULL ||
        !bsg_ksstring_extractHexValue(string, strlen(string), &address)) {
        return;
    }

    int limit = BSG_kDefaultMemorySearchDepth;
    bsg_kscrw_i_writeMemoryContents(writer, key, (uintptr_t)address, &limit);
}

#pragma mark Backtrace

/** Write a backtrace entry to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param address The memory address.
 *
 * @param info Information about the nearest symbols to the address.
 */
void bsg_kscrw_i_writeBacktraceEntry(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const uintptr_t address, const Dl_info *const info) {
    writer->beginObject(writer, key);
    {
        if (info->dli_fname != NULL) {
            writer->addStringElement(writer, BSG_KSCrashField_ObjectName,
                                     bsg_ksfulastPathEntry(info->dli_fname));
        }
        writer->addUIntegerElement(writer, BSG_KSCrashField_ObjectAddr,
                                   (uintptr_t)info->dli_fbase);
        if (info->dli_sname != NULL) {
            const char *sname = info->dli_sname;
            writer->addStringElement(writer, BSG_KSCrashField_SymbolName,
                                     sname);
        }
        writer->addUIntegerElement(writer, BSG_KSCrashField_SymbolAddr,
                                   (uintptr_t)info->dli_saddr);
        writer->addUIntegerElement(writer, BSG_KSCrashField_InstructionAddr,
                                   address);
    }
    writer->endContainer(writer);
}

/** Write a backtrace to the report.
 *
 * @param writer The writer to write the backtrace to.
 *
 * @param key The object key, if needed.
 *
 * @param backtrace The backtrace to write.
 *
 * @param backtraceLength Length of the backtrace.
 *
 * @param skippedEntries The number of entries that were skipped before the
 *                       beginning of backtrace.
 */
void bsg_kscrw_i_writeBacktrace(const BSG_KSCrashReportWriter *const writer,
                                const char *const key,
                                const uintptr_t *const backtrace,
                                const int backtraceLength,
                                const int skippedEntries) {
    writer->beginObject(writer, key);
    {
        writer->beginArray(writer, BSG_KSCrashField_Contents);
        {
            if (backtraceLength > 0) {
                Dl_info symbolicated[backtraceLength];
                bsg_ksbt_symbolicate(backtrace, symbolicated, backtraceLength,
                                     skippedEntries);

                for (int i = 0; i < backtraceLength; i++) {
                    bsg_kscrw_i_writeBacktraceEntry(writer, NULL, backtrace[i],
                                                    &symbolicated[i]);
                }
            }
        }
        writer->endContainer(writer);
        writer->addIntegerElement(writer, BSG_KSCrashField_Skipped,
                                  skippedEntries);
    }
    writer->endContainer(writer);
}

#pragma mark Stack

/** Write a dump of the stack contents to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param machineContext The context to retrieve the stack from.
 *
 * @param isStackOverflow If true, the stack has overflowed.
 */
void bsg_kscrw_i_writeStackContents(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const BSG_STRUCT_MCONTEXT_L *const machineContext,
    const bool isStackOverflow) {
    uintptr_t sp = bsg_ksmachstackPointer(machineContext);
    if ((void *)sp == NULL) {
        return;
    }

    uintptr_t lowAddress =
        sp + (uintptr_t)(BSG_kStackContentsPushedDistance * (int)sizeof(sp) *
                         bsg_ksmachstackGrowDirection() * -1);
    uintptr_t highAddress =
        sp + (uintptr_t)(BSG_kStackContentsPoppedDistance * (int)sizeof(sp) *
                         bsg_ksmachstackGrowDirection());
    if (highAddress < lowAddress) {
        uintptr_t tmp = lowAddress;
        lowAddress = highAddress;
        highAddress = tmp;
    }
    writer->beginObject(writer, key);
    {
        writer->addStringElement(writer, BSG_KSCrashField_GrowDirection,
                                 bsg_ksmachstackGrowDirection() > 0 ? "+"
                                                                    : "-");
        writer->addUIntegerElement(writer, BSG_KSCrashField_DumpStart,
                                   lowAddress);
        writer->addUIntegerElement(writer, BSG_KSCrashField_DumpEnd,
                                   highAddress);
        writer->addUIntegerElement(writer, BSG_KSCrashField_StackPtr, sp);
        writer->addBooleanElement(writer, BSG_KSCrashField_Overflow,
                                  isStackOverflow);
        uint8_t stackBuffer[BSG_kStackContentsTotalDistance * sizeof(sp)];
        size_t copyLength = highAddress - lowAddress;
        if (bsg_ksmachcopyMem((void *)lowAddress, stackBuffer, copyLength) ==
            KERN_SUCCESS) {
            writer->addDataElement(writer, BSG_KSCrashField_Contents,
                                   (void *)stackBuffer, copyLength);
        } else {
            writer->addStringElement(writer, BSG_KSCrashField_Error,
                                     "Stack contents not accessible");
        }
    }
    writer->endContainer(writer);
}

/** Write any notable addresses near the stack pointer (above and below).
 *
 * @param writer The writer.
 *
 * @param machineContext The context to retrieve the stack from.
 *
 * @param backDistance The distance towards the beginning of the stack to check.
 *
 * @param forwardDistance The distance past the end of the stack to check.
 */
void bsg_kscrw_i_writeNotableStackContents(
    const BSG_KSCrashReportWriter *const writer,
    const BSG_STRUCT_MCONTEXT_L *const machineContext, const int backDistance,
    const int forwardDistance) {
    uintptr_t sp = bsg_ksmachstackPointer(machineContext);
    if ((void *)sp == NULL) {
        return;
    }

    uintptr_t lowAddress =
        sp + (uintptr_t)(backDistance * (int)sizeof(sp) *
                         bsg_ksmachstackGrowDirection() * -1);
    uintptr_t highAddress = sp + (uintptr_t)(forwardDistance * (int)sizeof(sp) *
                                             bsg_ksmachstackGrowDirection());
    if (highAddress < lowAddress) {
        uintptr_t tmp = lowAddress;
        lowAddress = highAddress;
        highAddress = tmp;
    }
    uintptr_t contentsAsPointer;
    char nameBuffer[40];
    for (uintptr_t address = lowAddress; address < highAddress;
         address += sizeof(address)) {
        if (bsg_ksmachcopyMem((void *)address, &contentsAsPointer,
                              sizeof(contentsAsPointer)) == KERN_SUCCESS) {
            sprintf(nameBuffer, "stack@%p", (void *)address);
            bsg_kscrw_i_writeMemoryContentsIfNotable(writer, nameBuffer,
                                                     contentsAsPointer);
        }
    }
}

#pragma mark Registers

/** Write the contents of all regular registers to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param machineContext The context to retrieve the registers from.
 */
void bsg_kscrw_i_writeBasicRegisters(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    char registerNameBuff[30];
    const char *registerName;
    writer->beginObject(writer, key);
    {
        const int numRegisters = bsg_ksmachnumRegisters();
        for (int reg = 0; reg < numRegisters; reg++) {
            registerName = bsg_ksmachregisterName(reg);
            if (registerName == NULL) {
                snprintf(registerNameBuff, sizeof(registerNameBuff), "r%d",
                         reg);
                registerName = registerNameBuff;
            }
            writer->addUIntegerElement(
                writer, registerName,
                bsg_ksmachregisterValue(machineContext, reg));
        }
    }
    writer->endContainer(writer);
}

/** Write the contents of all exception registers to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param machineContext The context to retrieve the registers from.
 */
void bsg_kscrw_i_writeExceptionRegisters(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    char registerNameBuff[30];
    const char *registerName;
    writer->beginObject(writer, key);
    {
        const int numRegisters = bsg_ksmachnumExceptionRegisters();
        for (int reg = 0; reg < numRegisters; reg++) {
            registerName = bsg_ksmachexceptionRegisterName(reg);
            if (registerName == NULL) {
                snprintf(registerNameBuff, sizeof(registerNameBuff), "r%d",
                         reg);
                registerName = registerNameBuff;
            }
            writer->addUIntegerElement(
                writer, registerName,
                bsg_ksmachexceptionRegisterValue(machineContext, reg));
        }
    }
    writer->endContainer(writer);
}

/** Write all applicable registers.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param machineContext The context to retrieve the registers from.
 *
 * @param isCrashedContext If true, this context represents the crashing thread.
 */
void bsg_kscrw_i_writeRegisters(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const BSG_STRUCT_MCONTEXT_L *const machineContext,
    const bool isCrashedContext) {
    writer->beginObject(writer, key);
    {
        bsg_kscrw_i_writeBasicRegisters(writer, BSG_KSCrashField_Basic,
                                        machineContext);
        if (isCrashedContext) {
            bsg_kscrw_i_writeExceptionRegisters(
                writer, BSG_KSCrashField_Exception, machineContext);
        }
    }
    writer->endContainer(writer);
}

/** Write any notable addresses contained in the CPU registers.
 *
 * @param writer The writer.
 *
 * @param machineContext The context to retrieve the registers from.
 */
void bsg_kscrw_i_writeNotableRegisters(
    const BSG_KSCrashReportWriter *const writer,
    const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    char registerNameBuff[30];
    const char *registerName;
    const int numRegisters = bsg_ksmachnumRegisters();
    for (int reg = 0; reg < numRegisters; reg++) {
        registerName = bsg_ksmachregisterName(reg);
        if (registerName == NULL) {
            snprintf(registerNameBuff, sizeof(registerNameBuff), "r%d", reg);
            registerName = registerNameBuff;
        }
        bsg_kscrw_i_writeMemoryContentsIfNotable(
            writer, registerName,
            (uintptr_t)bsg_ksmachregisterValue(machineContext, reg));
    }
}

#pragma mark Thread-specific

/** Write any notable addresses in the stack or registers to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param machineContext The context to retrieve the registers from.
 */
void bsg_kscrw_i_writeNotableAddresses(
    const BSG_KSCrashReportWriter *const writer, const char *const key,
    const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    writer->beginObject(writer, key);
    {
        bsg_kscrw_i_writeNotableRegisters(writer, machineContext);
        bsg_kscrw_i_writeNotableStackContents(
            writer, machineContext, BSG_kStackNotableSearchBackDistance,
            BSG_kStackNotableSearchForwardDistance);
    }
    writer->endContainer(writer);
}

/** Write information about a thread to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param crash The crash handler context.
 *
 * @param thread The thread to write about.
 *
 * @param index The thread's index relative to all threads.
 *
 * @param writeNotableAddresses If true, write any notable addresses found.
 */
void bsg_kscrw_i_writeThread(const BSG_KSCrashReportWriter *const writer,
                             const char *const key,
                             const BSG_KSCrash_SentryContext *const crash,
                             const thread_t thread, const int index,
                             const bool writeNotableAddresses,
                             const bool searchThreadNames,
                             const bool searchQueueNames) {
    bool isCrashedThread = thread == crash->offendingThread;
    char nameBuffer[128];
    BSG_STRUCT_MCONTEXT_L machineContextBuffer;
    uintptr_t backtraceBuffer[BSG_kMaxBacktraceDepth];
    int backtraceLength = sizeof(backtraceBuffer) / sizeof(*backtraceBuffer);
    int skippedEntries = 0;

    BSG_STRUCT_MCONTEXT_L *machineContext =
        bsg_kscrw_i_getMachineContext(crash, thread, &machineContextBuffer);

    uintptr_t *backtrace =
        bsg_kscrw_i_getBacktrace(crash, thread, machineContext, backtraceBuffer,
                                 &backtraceLength, &skippedEntries);

    writer->beginObject(writer, key);
    {
        if (backtrace != NULL) {
            bsg_kscrw_i_writeBacktrace(writer, BSG_KSCrashField_Backtrace,
                                       backtrace, backtraceLength,
                                       skippedEntries);
        }
        if (machineContext != NULL) {
            bsg_kscrw_i_writeRegisters(writer, BSG_KSCrashField_Registers,
                                       machineContext, isCrashedThread);
        }
        writer->addIntegerElement(writer, BSG_KSCrashField_Index, index);
        if (searchThreadNames) {
            if (bsg_ksmachgetThreadName(thread, nameBuffer,
                                        sizeof(nameBuffer)) &&
                nameBuffer[0] != 0) {
                writer->addStringElement(writer, BSG_KSCrashField_Name,
                                         nameBuffer);
            }
        }
        if (searchQueueNames) {
            if (bsg_ksmachgetThreadQueueName(thread, nameBuffer,
                                             sizeof(nameBuffer)) &&
                nameBuffer[0] != 0) {
                writer->addStringElement(writer, BSG_KSCrashField_DispatchQueue,
                                         nameBuffer);
            }
        }
        writer->addBooleanElement(writer, BSG_KSCrashField_Crashed,
                                  isCrashedThread);
        writer->addBooleanElement(writer, BSG_KSCrashField_CurrentThread,
                                  thread == bsg_ksmachthread_self());
        if (isCrashedThread && machineContext != NULL) {
            bsg_kscrw_i_writeStackContents(writer, BSG_KSCrashField_Stack,
                                           machineContext, skippedEntries > 0);
            if (writeNotableAddresses) {
                bsg_kscrw_i_writeNotableAddresses(
                    writer, BSG_KSCrashField_NotableAddresses, machineContext);
            }
        }
    }
    writer->endContainer(writer);
}

/** Write information about all threads to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param crash The crash handler context.
 */
void bsg_kscrw_i_writeAllThreads(const BSG_KSCrashReportWriter *const writer,
                                 const char *const key,
                                 const BSG_KSCrash_SentryContext *const crash,
                                 bool writeNotableAddresses,
                                 bool searchThreadNames,
                                 bool searchQueueNames) {
    const task_t thisTask = mach_task_self();
    thread_act_array_t threads;
    mach_msg_type_number_t numThreads;
    kern_return_t kr;

    if ((kr = task_threads(thisTask, &threads, &numThreads)) != KERN_SUCCESS) {
        BSG_KSLOG_ERROR("task_threads: %s", mach_error_string(kr));
        return;
    }

    // Fetch info for all threads.
    writer->beginArray(writer, key);
    {
        for (mach_msg_type_number_t i = 0; i < numThreads; i++) {
            bsg_kscrw_i_writeThread(writer, NULL, crash, threads[i], (int)i,
                                    writeNotableAddresses, searchThreadNames,
                                    searchQueueNames);
        }
    }
    writer->endContainer(writer);

    // Clean up.
    for (mach_msg_type_number_t i = 0; i < numThreads; i++) {
        mach_port_deallocate(thisTask, threads[i]);
    }
    vm_deallocate(thisTask, (vm_address_t)threads,
                  sizeof(thread_t) * numThreads);
}

/** Get the index of a thread.
 *
 * @param thread The thread.
 *
 * @return The thread's index, or -1 if it couldn't be determined.
 */
int bsg_kscrw_i_threadIndex(const thread_t thread) {
    int index = -1;
    const task_t thisTask = mach_task_self();
    thread_act_array_t threads;
    mach_msg_type_number_t numThreads;
    kern_return_t kr;

    if ((kr = task_threads(thisTask, &threads, &numThreads)) != KERN_SUCCESS) {
        BSG_KSLOG_ERROR("task_threads: %s", mach_error_string(kr));
        return -1;
    }

    for (mach_msg_type_number_t i = 0; i < numThreads; i++) {
        if (threads[i] == thread) {
            index = (int)i;
            break;
        }
    }

    // Clean up.
    for (mach_msg_type_number_t i = 0; i < numThreads; i++) {
        mach_port_deallocate(thisTask, threads[i]);
    }
    vm_deallocate(thisTask, (vm_address_t)threads,
                  sizeof(thread_t) * numThreads);

    return index;
}

#pragma mark Global Report Data

/** Write information about a binary image to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param index Which image to write about.
 */
void bsg_kscrw_i_writeBinaryImage(const BSG_KSCrashReportWriter *const writer,
                                  const char *const key, const uint32_t index) {
    const struct mach_header *header = _dyld_get_image_header(index);
    if (header == NULL) {
        return;
    }

    uintptr_t cmdPtr = bsg_ksdlfirstCmdAfterHeader(header);
    if (cmdPtr == 0) {
        return;
    }

    // Look for the TEXT segment to get the image size.
    // Also look for a UUID command.
    uint64_t imageSize = 0;
    uint64_t imageVmAddr = 0;
    uint8_t *uuid = NULL;

    for (uint32_t iCmd = 0; iCmd < header->ncmds; iCmd++) {
        struct load_command *loadCmd = (struct load_command *)cmdPtr;
        switch (loadCmd->cmd) {
        case LC_SEGMENT: {
            struct segment_command *segCmd = (struct segment_command *)cmdPtr;
            if (strcmp(segCmd->segname, SEG_TEXT) == 0) {
                imageSize = segCmd->vmsize;
                imageVmAddr = segCmd->vmaddr;
            }
            break;
        }
        case LC_SEGMENT_64: {
            struct segment_command_64 *segCmd =
                (struct segment_command_64 *)cmdPtr;
            if (strcmp(segCmd->segname, SEG_TEXT) == 0) {
                imageSize = segCmd->vmsize;
                imageVmAddr = segCmd->vmaddr;
            }
            break;
        }
        case LC_UUID: {
            struct uuid_command *uuidCmd = (struct uuid_command *)cmdPtr;
            uuid = uuidCmd->uuid;
            break;
        }
        }
        cmdPtr += loadCmd->cmdsize;
    }

    writer->beginObject(writer, key);
    {
        writer->addUIntegerElement(writer, BSG_KSCrashField_ImageAddress,
                                   (uintptr_t)header);
        writer->addUIntegerElement(writer, BSG_KSCrashField_ImageVmAddress,
                                   imageVmAddr);
        writer->addUIntegerElement(writer, BSG_KSCrashField_ImageSize,
                                   imageSize);
        writer->addStringElement(writer, BSG_KSCrashField_Name,
                                 _dyld_get_image_name(index));
        writer->addUUIDElement(writer, BSG_KSCrashField_UUID, uuid);
        writer->addIntegerElement(writer, BSG_KSCrashField_CPUType,
                                  header->cputype);
        writer->addIntegerElement(writer, BSG_KSCrashField_CPUSubType,
                                  header->cpusubtype);
    }
    writer->endContainer(writer);
}

/** Write information about all images to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 */
void bsg_kscrw_i_writeBinaryImages(const BSG_KSCrashReportWriter *const writer,
                                   const char *const key) {
    const uint32_t imageCount = _dyld_image_count();

    writer->beginArray(writer, key);
    {
        for (uint32_t iImg = 0; iImg < imageCount; iImg++) {
            bsg_kscrw_i_writeBinaryImage(writer, NULL, iImg);
        }
    }
    writer->endContainer(writer);
}

/** Write information about system memory to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 */
void bsg_kscrw_i_writeMemoryInfo(const BSG_KSCrashReportWriter *const writer,
                                 const char *const key) {
    writer->beginObject(writer, key);
    {
        writer->addUIntegerElement(writer, BSG_KSCrashField_Usable,
                                   bsg_ksmachusableMemory());
        writer->addUIntegerElement(writer, BSG_KSCrashField_Free,
                                   bsg_ksmachfreeMemory());
    }
    writer->endContainer(writer);
}

/** Write information about the error leading to the crash to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param crash The crash handler context.
 */
void bsg_kscrw_i_writeError(const BSG_KSCrashReportWriter *const writer,
                            const char *const key,
                            const BSG_KSCrash_SentryContext *const crash) {
    int machExceptionType = 0;
    kern_return_t machCode = 0;
    kern_return_t machSubCode = 0;
    int sigNum = 0;
    int sigCode = 0;
    const char *exceptionName = NULL;
    const char *crashReason = NULL;

    // Gather common info.
    switch (crash->crashType) {
    case BSG_KSCrashTypeMainThreadDeadlock:
        break;
    case BSG_KSCrashTypeMachException:
        machExceptionType = crash->mach.type;
        machCode = (kern_return_t)crash->mach.code;
        if (machCode == KERN_PROTECTION_FAILURE && crash->isStackOverflow) {
            // A stack overflow should return KERN_INVALID_ADDRESS, but
            // when a stack blasts through the guard pages at the top of the
            // stack, it generates KERN_PROTECTION_FAILURE. Correct for this.
            machCode = KERN_INVALID_ADDRESS;
        }
        machSubCode = (kern_return_t)crash->mach.subcode;

        sigNum =
            bsg_kssignal_signalForMachException(machExceptionType, machCode);
        break;
    case BSG_KSCrashTypeCPPException:
        machExceptionType = EXC_CRASH;
        sigNum = SIGABRT;
        crashReason = crash->crashReason;
        exceptionName = crash->CPPException.name;
        break;
    case BSG_KSCrashTypeNSException:
        machExceptionType = EXC_CRASH;
        sigNum = SIGABRT;
        exceptionName = crash->NSException.name;
        crashReason = crash->crashReason;
        break;
    case BSG_KSCrashTypeSignal:
        sigNum = crash->signal.signalInfo->si_signo;
        sigCode = crash->signal.signalInfo->si_code;
        machExceptionType = bsg_kssignal_machExceptionForSignal(sigNum);
        break;
    case BSG_KSCrashTypeUserReported:
        machExceptionType = EXC_CRASH;
        sigNum = SIGABRT;
        crashReason = crash->crashReason;
        break;
    }

    const char *machExceptionName = bsg_ksmachexceptionName(machExceptionType);
    const char *machCodeName =
        machCode == 0 ? NULL : bsg_ksmachkernelReturnCodeName(machCode);
    const char *sigName = bsg_kssignal_signalName(sigNum);
    const char *sigCodeName = bsg_kssignal_signalCodeName(sigNum, sigCode);

    writer->beginObject(writer, key);
    {

        if (BSG_KSCrashTypeUserReported != crash->crashType) {
            writer->addUIntegerElement(writer, BSG_KSCrashField_Address,
                                       crash->faultAddress);
        }

        if (crashReason != NULL) {
            writer->addStringElement(writer, BSG_KSCrashField_Reason,
                                     crashReason);
        }


        // Gather specific info.
        switch (crash->crashType) {
        case BSG_KSCrashTypeMainThreadDeadlock:
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashExcType_Deadlock);
            break;

        case BSG_KSCrashTypeMachException:
            writer->beginObject(writer, BSG_KSCrashField_Mach);
            {
                writer->addUIntegerElement(writer, BSG_KSCrashField_Exception,
                                           (unsigned)machExceptionType);
                if (machExceptionName != NULL) {
                    writer->addStringElement(writer, BSG_KSCrashField_ExceptionName,
                                             machExceptionName);
                }
                writer->addUIntegerElement(writer, BSG_KSCrashField_Code,
                                           (unsigned)machCode);
                if (machCodeName != NULL) {
                    writer->addStringElement(writer, BSG_KSCrashField_CodeName,
                                             machCodeName);
                }
                writer->addUIntegerElement(writer, BSG_KSCrashField_Subcode,
                                           (unsigned)machSubCode);
            }
            writer->endContainer(writer);
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashExcType_Mach);
            break;

        case BSG_KSCrashTypeCPPException: {
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashExcType_CPPException);
            writer->beginObject(writer, BSG_KSCrashField_CPPException);
            {
                writer->addStringElement(writer, BSG_KSCrashField_Name,
                                         exceptionName);
            }
            writer->endContainer(writer);
            break;
        }
        case BSG_KSCrashTypeNSException: {
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashExcType_NSException);
            writer->beginObject(writer, BSG_KSCrashField_NSException);
            {
                writer->addStringElement(writer, BSG_KSCrashField_Name,
                                         exceptionName);
                bsg_kscrw_i_writeAddressReferencedByString(
                    writer, BSG_KSCrashField_ReferencedObject, crashReason);
            }
            writer->endContainer(writer);
            break;
        }
        case BSG_KSCrashTypeSignal:
            writer->beginObject(writer, BSG_KSCrashField_Signal);
            {
                writer->addUIntegerElement(writer, BSG_KSCrashField_Signal,
                                           (unsigned)sigNum);
                if (sigName != NULL) {
                    writer->addStringElement(writer, BSG_KSCrashField_Name,
                                             sigName);
                }
                writer->addUIntegerElement(writer, BSG_KSCrashField_Code,
                                           (unsigned)sigCode);
                if (sigCodeName != NULL) {
                    writer->addStringElement(writer, BSG_KSCrashField_CodeName,
                                             sigCodeName);
                }
            }
            writer->endContainer(writer);
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashExcType_Signal);
            break;

        case BSG_KSCrashTypeUserReported: {
            writer->addStringElement(writer, BSG_KSCrashField_Type,
                                     BSG_KSCrashExcType_User);
            writer->beginObject(writer, BSG_KSCrashField_UserReported);
            {
                writer->addStringElement(writer, BSG_KSCrashField_Name,
                                         crash->userException.name);
            }
            writer->endContainer(writer);
            break;
        }
        }
    }
    writer->endContainer(writer);
}

/** Write information about app runtime, etc to the report.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param state The persistent crash handler state.
 */
void bsg_kscrw_i_writeAppStats(const BSG_KSCrashReportWriter *const writer,
                               const char *const key,
                               BSG_KSCrash_State *state) {
    writer->beginObject(writer, key);
    {
        writer->addBooleanElement(writer, BSG_KSCrashField_AppActive,
                                  state->applicationIsActive);
        writer->addBooleanElement(writer, BSG_KSCrashField_AppInFG,
                                  state->applicationIsInForeground);

        writer->addIntegerElement(writer, BSG_KSCrashField_LaunchesSinceCrash,
                                  state->launchesSinceLastCrash);
        writer->addIntegerElement(writer, BSG_KSCrashField_SessionsSinceCrash,
                                  state->sessionsSinceLastCrash);
        writer->addFloatingPointElement(writer,
                                        BSG_KSCrashField_ActiveTimeSinceCrash,
                                        state->activeDurationSinceLastCrash);
        writer->addFloatingPointElement(
            writer, BSG_KSCrashField_BGTimeSinceCrash,
            state->backgroundDurationSinceLastCrash);

        writer->addIntegerElement(writer, BSG_KSCrashField_SessionsSinceLaunch,
                                  state->sessionsSinceLaunch);
        writer->addFloatingPointElement(writer,
                                        BSG_KSCrashField_ActiveTimeSinceLaunch,
                                        state->activeDurationSinceLaunch);
        writer->addFloatingPointElement(writer,
                                        BSG_KSCrashField_BGTimeSinceLaunch,
                                        state->backgroundDurationSinceLaunch);
    }
    writer->endContainer(writer);
}

/** Write information about this process.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 */
void bsg_kscrw_i_writeProcessState(const BSG_KSCrashReportWriter *const writer,
                                   const char *const key) {
    writer->beginObject(writer, key);
    {
        const void *excAddress = bsg_kszombie_lastDeallocedNSExceptionAddress();
        if (excAddress != NULL) {
            writer->beginObject(writer,
                                BSG_KSCrashField_LastDeallocedNSException);
            {
                writer->addUIntegerElement(writer, BSG_KSCrashField_Address,
                                           (uintptr_t)excAddress);
                writer->addStringElement(
                    writer, BSG_KSCrashField_Name,
                    bsg_kszombie_lastDeallocedNSExceptionName());
                writer->addStringElement(
                    writer, BSG_KSCrashField_Reason,
                    bsg_kszombie_lastDeallocedNSExceptionReason());
                bsg_kscrw_i_writeAddressReferencedByString(
                    writer, BSG_KSCrashField_ReferencedObject,
                    bsg_kszombie_lastDeallocedNSExceptionReason());
            }
            writer->endContainer(writer);
        }
    }
    writer->endContainer(writer);
}

/** Write basic report information.
 *
 * @param writer The writer.
 *
 * @param key The object key, if needed.
 *
 * @param type The report type.
 *
 * @param reportID The report ID.
 */
void bsg_kscrw_i_writeReportInfo(const BSG_KSCrashReportWriter *const writer,
                                 const char *const key, const char *const type,
                                 const char *const reportID,
                                 const char *const processName) {
    writer->beginObject(writer, key);
    {
        writer->addStringElement(writer, BSG_KSCrashField_Version,
                                 BSG_KSCRASH_REPORT_VERSION);
        writer->addStringElement(writer, BSG_KSCrashField_ID, reportID);
        writer->addStringElement(writer, BSG_KSCrashField_ProcessName,
                                 processName);
        writer->addIntegerElement(writer, BSG_KSCrashField_Timestamp,
                                  time(NULL));
        writer->addStringElement(writer, BSG_KSCrashField_Type, type);
    }
    writer->endContainer(writer);
}

#pragma mark Setup

/** Prepare a report writer for use.
 *
 * @oaram writer The writer to prepare.
 *
 * @param context JSON writer contextual information.
 */
void bsg_kscrw_i_prepareReportWriter(BSG_KSCrashReportWriter *const writer,
                                     BSG_KSJSONEncodeContext *const context) {
    writer->addBooleanElement = bsg_kscrw_i_addBooleanElement;
    writer->addFloatingPointElement = bsg_kscrw_i_addFloatingPointElement;
    writer->addIntegerElement = bsg_kscrw_i_addIntegerElement;
    writer->addUIntegerElement = bsg_kscrw_i_addUIntegerElement;
    writer->addStringElement = bsg_kscrw_i_addStringElement;
    writer->addTextFileElement = bsg_kscrw_i_addTextFileElement;
    writer->addJSONFileElement = bsg_kscrw_i_addJSONElementFromFile;
    writer->addDataElement = bsg_kscrw_i_addDataElement;
    writer->beginDataElement = bsg_kscrw_i_beginDataElement;
    writer->appendDataElement = bsg_kscrw_i_appendDataElement;
    writer->endDataElement = bsg_kscrw_i_endDataElement;
    writer->addUUIDElement = bsg_kscrw_i_addUUIDElement;
    writer->addJSONElement = bsg_kscrw_i_addJSONElement;
    writer->beginObject = bsg_kscrw_i_beginObject;
    writer->beginArray = bsg_kscrw_i_beginArray;
    writer->endContainer = bsg_kscrw_i_endContainer;
    writer->context = context;
}

/** Open the crash report file.
 *
 * @param path The path to the file.
 *
 * @return The file descriptor, or -1 if an error occurred.
 */
int bsg_kscrw_i_openCrashReportFile(const char *const path) {
    int fd = open(path, O_RDWR | O_CREAT | O_EXCL, 0644);
    if (fd < 0) {
        BSG_KSLOG_ERROR("Could not open crash report file %s: %s", path,
                        strerror(errno));
    }
    return fd;
}

/** Record whether the crashed thread had a stack overflow or not.
 *
 * @param crashContext the context.
 */
void bsg_kscrw_i_updateStackOverflowStatus(
    BSG_KSCrash_Context *const crashContext) {
    // TODO: This feels weird. Shouldn't be mutating the context.
    if (bsg_kscrw_i_isStackOverflow(&crashContext->crash,
                                    crashContext->crash.offendingThread)) {
        BSG_KSLOG_TRACE("Stack overflow detected.");
        crashContext->crash.isStackOverflow = true;
    }
}

void bsg_kscrw_i_callUserCrashHandler(BSG_KSCrash_Context *const crashContext,
                                      BSG_KSCrashReportWriter *writer) {
    BSG_KSCrashType type = crashContext->crash.crashType;
    crashContext->config.onCrashNotify(writer, type);
}

// ============================================================================
#pragma mark - Main API -
// ============================================================================

void bsg_kscrashreport_writeMinimalReport(
    BSG_KSCrash_Context *const crashContext, const char *const path) {
    BSG_KSLOG_INFO("Writing minimal crash report to %s", path);

    int fd = bsg_kscrw_i_openCrashReportFile(path);
    if (fd < 0) {
        return;
    }

    bsg_g_introspectionRules = &crashContext->config.introspectionRules;

    bsg_kscrw_i_updateStackOverflowStatus(crashContext);

    BSG_KSJSONEncodeContext jsonContext;
    jsonContext.userData = &fd;
    BSG_KSCrashReportWriter concreteWriter;
    BSG_KSCrashReportWriter *writer = &concreteWriter;
    bsg_kscrw_i_prepareReportWriter(writer, &jsonContext);

    bsg_ksjsonbeginEncode(bsg_getJsonContext(writer), true,
                          bsg_kscrw_i_addJSONData, &fd);

    writer->beginObject(writer, BSG_KSCrashField_Report);
    {
        bsg_kscrw_i_writeReportInfo(
            writer, BSG_KSCrashField_Report, BSG_KSCrashReportType_Minimal,
            crashContext->config.crashID, crashContext->config.processName);

        writer->beginObject(writer, BSG_KSCrashField_Crash);
        {
            bsg_kscrw_i_writeThread(
                writer, BSG_KSCrashField_CrashedThread, &crashContext->crash,
                crashContext->crash.offendingThread,
                bsg_kscrw_i_threadIndex(crashContext->crash.offendingThread),
                false, false, false);
            bsg_kscrw_i_writeError(writer, BSG_KSCrashField_Error,
                                   &crashContext->crash);
        }
        writer->endContainer(writer);
    }
    writer->endContainer(writer);

    bsg_ksjsonendEncode(bsg_getJsonContext(writer));

    close(fd);
}

void bsg_kscrashreport_writeStandardReport(
    BSG_KSCrash_Context *const crashContext, const char *const path) {
    BSG_KSLOG_INFO("Writing crash report to %s", path);

    int fd = bsg_kscrw_i_openCrashReportFile(path);
    if (fd < 0) {
        return;
    }

    bsg_g_introspectionRules = &crashContext->config.introspectionRules;

    bsg_kscrw_i_updateStackOverflowStatus(crashContext);

    BSG_KSJSONEncodeContext jsonContext;
    jsonContext.userData = &fd;
    BSG_KSCrashReportWriter concreteWriter;
    BSG_KSCrashReportWriter *writer = &concreteWriter;
    bsg_kscrw_i_prepareReportWriter(writer, &jsonContext);

    bsg_ksjsonbeginEncode(bsg_getJsonContext(writer), true,
                          bsg_kscrw_i_addJSONData, &fd);

    writer->beginObject(writer, BSG_KSCrashField_Report);
    {
        bsg_kscrw_i_writeReportInfo(
            writer, BSG_KSCrashField_Report, BSG_KSCrashReportType_Standard,
            crashContext->config.crashID, crashContext->config.processName);

        // Don't write the binary images for user reported crashes to improve
        // performance
        if (crashContext->crash.writeBinaryImagesForUserReported == true ||
            crashContext->crash.crashType != BSG_KSCrashTypeUserReported) {
            bsg_kscrw_i_writeBinaryImages(writer,
                                          BSG_KSCrashField_BinaryImages);
        }

        bsg_kscrw_i_writeProcessState(writer, BSG_KSCrashField_ProcessState);

        if (crashContext->config.systemInfoJSON != NULL) {
            bsg_kscrw_i_addJSONElement(writer, BSG_KSCrashField_System,
                                       crashContext->config.systemInfoJSON);
        }

        writer->beginObject(writer, BSG_KSCrashField_SystemAtCrash);
        {
            bsg_kscrw_i_writeMemoryInfo(writer, BSG_KSCrashField_Memory);
            bsg_kscrw_i_writeAppStats(writer, BSG_KSCrashField_AppStats,
                                      &crashContext->state);
        }
        writer->endContainer(writer);

        if (crashContext->config.userInfoJSON != NULL) {
            bsg_kscrw_i_addJSONElement(writer, BSG_KSCrashField_User,
                                       crashContext->config.userInfoJSON);
        }

        writer->beginObject(writer, BSG_KSCrashField_Crash);
        {
            // Don't write the threads for user reported crashes to improve
            // performance
            if (crashContext->crash.threadTracingEnabled == true ||
                crashContext->crash.crashType != BSG_KSCrashTypeUserReported) {
                bsg_kscrw_i_writeAllThreads(
                    writer, BSG_KSCrashField_Threads, &crashContext->crash,
                    crashContext->config.introspectionRules.enabled,
                    crashContext->config.searchThreadNames,
                    crashContext->config.searchQueueNames);
            }
            bsg_kscrw_i_writeError(writer, BSG_KSCrashField_Error,
                    &crashContext->crash);
        }
        writer->endContainer(writer);

        if (crashContext->config.onCrashNotify != NULL) {
            // Write handled exception report info
            writer->beginObject(writer, BSG_KSCrashField_UserAtCrash);
            if (crashContext->crash.crashType == BSG_KSCrashTypeUserReported) {
                if (crashContext->crash.userException.overrides != NULL) {
                   writer->addJSONElement(writer, BSG_KSCrashField_Overrides,
                                          crashContext->crash.userException.overrides);
                }
                if (crashContext->crash.userException.handledState != NULL) {
                    writer->addJSONElement(writer, BSG_KSCrashField_HandledState,
                                           crashContext->crash.userException.handledState);
                }
                if (crashContext->crash.userException.metadata != NULL) {
                    writer->addJSONElement(writer, BSG_KSCrashField_Metadata,
                                           crashContext->crash.userException.metadata);
                }
                if (crashContext->crash.userException.state != NULL) {
                    writer->addJSONElement(writer, BSG_KSCrashField_State,
                                           crashContext->crash.userException.state);
                }
                if (crashContext->crash.userException.config != NULL) {
                    writer->addJSONElement(writer, BSG_KSCrashField_Config,
                                           crashContext->crash.userException.config);
                }
                writer->addIntegerElement(writer, BSG_KSCrashField_DiscardDepth,
                                       crashContext->crash.userException.discardDepth);
            }
            { bsg_kscrw_i_callUserCrashHandler(crashContext, writer); }
            writer->endContainer(writer);
        }
    }
    writer->endContainer(writer);

    bsg_ksjsonendEncode(bsg_getJsonContext(writer));

    if (!bsg_ksfuflushWriteBuffer(fd)) {
        BSG_KSLOG_ERROR("Failed to flush write buffer");
    }
    close(fd);
}

void bsg_kscrashreport_logCrash(const BSG_KSCrash_Context *const crashContext) {
    const BSG_KSCrash_SentryContext *crash = &crashContext->crash;
    bsg_kscrw_i_logCrashType(crash);
    bsg_kscrw_i_logCrashThreadBacktrace(&crashContext->crash);
}
