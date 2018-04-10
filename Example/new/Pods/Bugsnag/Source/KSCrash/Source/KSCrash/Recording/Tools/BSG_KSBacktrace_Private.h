//
//  KSBacktrace_Private.h
//
//  Created by Karl Stenerud on 2012-01-29.
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

#ifndef HDR_BSG_KSBacktrace_private_h
#define HDR_BSG_KSBacktrace_private_h

#ifdef __cplusplus
extern "C" {
#endif

#include "BSG_KSArchSpecific.h"
#include "BSG_KSBacktrace.h"

#include <sys/ucontext.h>

/** Point at which bsg_ksbt_backtraceLength() will give up trying to count.
 *
 * This really only comes into play during a stack overflow.
 */
#define BSG_kBacktraceGiveUpPoint 10000000

/** Count how many entries there are in a potential backtrace.
 *
 * This is useful for intelligently generating a backtrace after a stack
 * overflow.
 *
 * @param machineContext The machine context to check the backtrace for.
 *
 * @return The number of backtrace entries.
 */
int bsg_ksbt_backtraceLength(const BSG_STRUCT_MCONTEXT_L *machineContext);

/** Check if a backtrace is too long.
 *
 * @param machineContext The machine context to check the backtrace for.
 *
 * @param maxLength The give up point.
 *
 * @return true if the backtrace is longer than maxLength.
 */
bool bsg_ksbt_isBacktraceTooLong(
    const BSG_STRUCT_MCONTEXT_L *const machineContext, int maxLength);

/** Generate a backtrace using the thread state in the specified machine context
 *  (async-safe).
 *
 *
 * @param machineContext The machine context to generate a backtrace for.
 *
 * @param backtraceBuffer A buffer to hold the backtrace.
 *
 * @param maxEntries The maximum number of trace entries to generate (must not
 *                   be larger than backtrace_buff can hold).
 *
 * @return The number of backtrace entries generated.
 */
int bsg_ksbt_backtraceThreadState(const BSG_STRUCT_MCONTEXT_L *machineContext,
                                  uintptr_t *backtraceBuffer, int skipEntries,
                                  int maxEntries);

#ifdef __cplusplus
}
#endif

#endif // HDR_KSBacktrace_private_h
