//
//  BSG_KSLogger.h
//
//  Created by Karl Stenerud on 11-06-25.
//
//  Copyright (c) 2011 Karl Stenerud. All rights reserved.
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

/**
 * BSG_KSLogger
 * ========
 *
 * Prints log entries to the console consisting of:
 * - Level (Error, Warn, Info, Debug, Trace)
 * - File
 * - Line
 * - Function
 * - Message
 *
 * Allows setting the minimum logging level in the preprocessor.
 *
 * Works in C or Objective-C contexts, with or without ARC, using CLANG or GCC.
 *
 *
 * =====
 * USAGE
 * =====
 *
 * Set the log level in your "Preprocessor Macros" build setting. You may choose
 * TRACE, DEBUG, INFO, WARN, ERROR. If nothing is set, it defaults to INFO.
 *
 * Example: BSG_KSLogger_Level=WARN
 *
 * Anything below the level specified for BSG_KSLogger_Level will not be
 * compiled or printed.
 *
 *
 * Next, include the header file:
 *
 * #include "BSG_KSLogger.h"
 *
 *
 * Next, call the logger functions from your code (using objective-c strings
 * in objective-C files and regular strings in regular C files):
 *
 * Code:
 *    BSG_KSLOG_ERROR(@"Some error message");
 *
 * Prints:
 *    2011-07-16 05:41:01.379 TestApp[4439:f803] ERROR: SomeClass.m (21):
 * -[SomeFunction]: Some error message
 *
 * Code:
 *    BSG_KSLOG_INFO(@"Info about %@", someObject);
 *
 * Prints:
 *    2011-07-16 05:44:05.239 TestApp[4473:f803] INFO : SomeClass.m (20):
 * -[SomeFunction]: Info about <NSObject: 0xb622840>
 *
 *
 * The "BASIC" versions of the macros behave exactly like NSLog() or printf(),
 * except they respect the BSG_KSLogger_Level setting:
 *
 * Code:
 *    BSG_KSLOGBASIC_ERROR(@"A basic log entry");
 *
 * Prints:
 *    2011-07-16 05:44:05.916 TestApp[4473:f803] A basic log entry
 *
 *
 * NOTE: In C files, use "" instead of @"" in the format field. Logging calls
 *       in C files do not print the NSLog preamble:
 *
 * Objective-C version:
 *    BSG_KSLOG_ERROR(@"Some error message");
 *
 *    2011-07-16 05:41:01.379 TestApp[4439:f803] ERROR: SomeClass.m (21):
 * -[SomeFunction]: Some error message
 *
 * C version:
 *    BSG_KSLOG_ERROR("Some error message");
 *
 *    ERROR: SomeClass.c (21): SomeFunction(): Some error message
 *
 *
 * =============
 * LOCAL LOGGING
 * =============
 *
 * You can control logging messages at the local file level using the
 * "KSLogger_LocalLevel" define. Note that it must be defined BEFORE
 * including BSG_KSLogger.h
 *
 * The BSG_KSLOG_XX() and BSG_KSLOGBASIC_XX() macros will print out based on the
 * LOWER of BSG_KSLogger_Level and BSG_KSLogger_LocalLevel, so if
 * BSG_KSLogger_Level is DEBUG and BSG_KSLogger_LocalLevel is TRACE, it will
 * print all the way down to the trace level for the local file where
 * BSG_KSLogger_LocalLevel was defined, and to the debug level everywhere else.
 *
 * Example:
 *
 * // BSG_KSLogger_LocalLevel, if defined, MUST come BEFORE including
 * BSG_KSLogger.h #define BSG_KSLogger_LocalLevel TRACE #import "BSG_KSLogger.h"
 *
 *
 * ===============
 * IMPORTANT NOTES
 * ===============
 *
 * The C logger changes its behavior depending on the value of the preprocessor
 * define BSG_KSLogger_CBufferSize.
 *
 * If BSG_KSLogger_CBufferSize is > 0, the C logger will behave in an async-safe
 * manner, calling write() instead of printf(). Any log messages that exceed the
 * length specified by BSG_KSLogger_CBufferSize will be truncated.
 *
 * If BSG_KSLogger_CBufferSize == 0, the C logger will use printf(), and there
 * will be no limit on the log message length.
 *
 * BSG_KSLogger_CBufferSize can only be set as a preprocessor define, and will
 * default to 1024 if not specified during compilation.
 */

// ============================================================================
#pragma mark - (internal) -
// ============================================================================

#ifndef HDR_BSG_KSLogger_h
#define HDR_BSG_KSLogger_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#ifdef __OBJC__

#import <Foundation/Foundation.h>

void bsg_i_kslog_logObjC(const char *level, const char *file, int line,
                         const char *function, NSString *fmt, ...);

void bsg_i_kslog_logObjCBasic(NSString *fmt, ...);

#define i_KSLOG_FULL bsg_i_kslog_logObjC
#define i_KSLOG_BASIC bsg_i_kslog_logObjCBasic

#else // __OBJC__

void bsg_i_kslog_logC(const char *level, const char *file, int line,
                      const char *function, const char *fmt, ...);

void bsg_i_kslog_logCBasic(const char *fmt, ...);

#define i_KSLOG_FULL bsg_i_kslog_logC
#define i_KSLOG_BASIC bsg_i_kslog_logCBasic

#endif // __OBJC__

/* Back up any existing defines by the same name */
#ifdef NONE
#define BSG_KSLOG_BAK_NONE NONE
#undef NONE
#endif
#ifdef ERROR
#define BSG_KSLOG_BAK_ERROR ERROR
#undef ERROR
#endif
#ifdef WARN
#define BSG_KSLOG_BAK_WARN WARN
#undef WARN
#endif
#ifdef INFO
#define BSG_KSLOG_BAK_INFO INFO
#undef INFO
#endif
#ifdef DEBUG
#define BSG_KSLOG_BAK_DEBUG DEBUG
#undef DEBUG
#endif
#ifdef TRACE
#define BSG_KSLOG_BAK_TRACE TRACE
#undef TRACE
#endif

#define BSG_KSLogger_Level_None 0
#define BSG_KSLogger_Level_Error 10
#define BSG_KSLogger_Level_Warn 20
#define BSG_KSLogger_Level_Info 30
#define BSG_KSLogger_Level_Debug 40
#define BSG_KSLogger_Level_Trace 50

#define NONE BSG_KSLogger_Level_None
#define ERROR BSG_KSLogger_Level_Error
#define WARN BSG_KSLogger_Level_Warn
#define INFO BSG_KSLogger_Level_Info
#define DEBUG BSG_KSLogger_Level_Debug
#define TRACE BSG_KSLogger_Level_Trace

#ifndef BSG_KSLogger_Level
#define BSG_KSLogger_Level BSG_KSLogger_Level_Info
#endif

#ifndef BSG_KSLogger_LocalLevel
#define BSG_KSLogger_LocalLevel BSG_KSLogger_Level_None
#endif

#define a_KSLOG_FULL(LEVEL, FMT, ...)                                          \
    i_KSLOG_FULL(LEVEL, __FILE__, __LINE__, __PRETTY_FUNCTION__, FMT,          \
                 ##__VA_ARGS__)

// ============================================================================
#pragma mark - API -
// ============================================================================

/** Set the filename to log to.
 *
 * @param filename The file to write to (NULL = write to stdout).
 *
 * @param overwrite If true, overwrite the log file.
 */
bool bsg_kslog_setLogFilename(const char *filename, bool overwrite);

/** Tests if the logger would print at the specified level.
 *
 * @param LEVEL The level to test for. One of:
 *            BSG_KSLogger_Level_Error,
 *            BSG_KSLogger_Level_Warn,
 *            BSG_KSLogger_Level_Info,
 *            BSG_KSLogger_Level_Debug,
 *            BSG_KSLogger_Level_Trace,
 *
 * @return TRUE if the logger would print at the specified level.
 */
#define BSG_KSLOG_PRINTS_AT_LEVEL(LEVEL)                                       \
    (BSG_KSLogger_Level >= LEVEL || BSG_KSLogger_LocalLevel >= LEVEL)

/** Log a message regardless of the log settings.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#define BSG_KSLOG_ALWAYS(FMT, ...) a_KSLOG_FULL("FORCE", FMT, ##__VA_ARGS__)
#define BSG_KSLOGBASIC_ALWAYS(FMT, ...) i_KSLOG_BASIC(FMT, ##__VA_ARGS__)

/** Log an error.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if BSG_KSLOG_PRINTS_AT_LEVEL(BSG_KSLogger_Level_Error)
#define BSG_KSLOG_ERROR(FMT, ...) a_KSLOG_FULL("ERROR", FMT, ##__VA_ARGS__)
#define BSG_KSLOGBASIC_ERROR(FMT, ...) i_KSLOG_BASIC(FMT, ##__VA_ARGS__)
#else
#define BSG_KSLOG_ERROR(FMT, ...)
#define BSG_KSLOGBASIC_ERROR(FMT, ...)
#endif

/** Log a warning.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if BSG_KSLOG_PRINTS_AT_LEVEL(BSG_KSLogger_Level_Warn)
#define BSG_KSLOG_WARN(FMT, ...) a_KSLOG_FULL("WARN ", FMT, ##__VA_ARGS__)
#define BSG_KSLOGBASIC_WARN(FMT, ...) i_KSLOG_BASIC(FMT, ##__VA_ARGS__)
#else
#define BSG_KSLOG_WARN(FMT, ...)
#define BSG_KSLOGBASIC_WARN(FMT, ...)
#endif

/** Log an info message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if BSG_KSLOG_PRINTS_AT_LEVEL(BSG_KSLogger_Level_Info)
#define BSG_KSLOG_INFO(FMT, ...) a_KSLOG_FULL("INFO ", FMT, ##__VA_ARGS__)
#define BSG_KSLOGBASIC_INFO(FMT, ...) i_KSLOG_BASIC(FMT, ##__VA_ARGS__)
#else
#define BSG_KSLOG_INFO(FMT, ...)
#define BSG_KSLOGBASIC_INFO(FMT, ...)
#endif

/** Log a debug message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if BSG_KSLOG_PRINTS_AT_LEVEL(BSG_KSLogger_Level_Debug)
#define BSG_KSLOG_DEBUG(FMT, ...) a_KSLOG_FULL("DEBUG", FMT, ##__VA_ARGS__)
#define BSG_KSLOGBASIC_DEBUG(FMT, ...) i_KSLOG_BASIC(FMT, ##__VA_ARGS__)
#else
#define BSG_KSLOG_DEBUG(FMT, ...)
#define BSG_KSLOGBASIC_DEBUG(FMT, ...)
#endif

/** Log a trace message.
 * Normal version prints out full context. Basic version prints directly.
 *
 * @param FMT The format specifier, followed by its arguments.
 */
#if BSG_KSLOG_PRINTS_AT_LEVEL(BSG_KSLogger_Level_Trace)
#define BSG_KSLOG_TRACE(FMT, ...) a_KSLOG_FULL("TRACE", FMT, ##__VA_ARGS__)
#define BSG_KSLOGBASIC_TRACE(FMT, ...) i_KSLOG_BASIC(FMT, ##__VA_ARGS__)
#else
#define BSG_KSLOG_TRACE(FMT, ...)
#define BSG_KSLOGBASIC_TRACE(FMT, ...)
#endif

// ============================================================================
#pragma mark - (internal) -
// ============================================================================

/* Put everything back to the way we found it. */
#undef ERROR
#ifdef BSG_KSLOG_BAK_ERROR
#define ERROR BSG_KSLOG_BAK_ERROR
#undef BSG_KSLOG_BAK_ERROR
#endif
#undef WARNING
#ifdef BSG_KSLOG_BAK_WARN
#define WARNING BSG_KSLOG_BAK_WARN
#undef BSG_KSLOG_BAK_WARN
#endif
#undef INFO
#ifdef BSG_KSLOG_BAK_INFO
#define INFO BSG_KSLOG_BAK_INFO
#undef BSG_KSLOG_BAK_INFO
#endif
#undef DEBUG
#ifdef BSG_KSLOG_BAK_DEBUG
#define DEBUG BSG_KSLOG_BAK_DEBUG
#undef BSG_KSLOG_BAK_DEBUG
#endif
#undef TRACE
#ifdef BSG_KSLOG_BAK_TRACE
#define TRACE BSG_KSLOG_BAK_TRACE
#undef BSG_KSLOG_BAK_TRACE
#endif

#ifdef __cplusplus
}
#endif

#endif // HDR_KSLogger_h
