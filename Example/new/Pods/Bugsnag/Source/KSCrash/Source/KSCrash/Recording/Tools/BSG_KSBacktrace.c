//
//  KSBacktrace.c
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

#include "BSG_KSBacktrace_Private.h"

#include "BSG_KSDynamicLinker.h"
#include "BSG_KSMach.h"

/**
 * Mask to strip pointer authentication codes from pointers on Arm64e
 * devices. Example usage, assuming the usage is guarded for __arm64__:
 *     uintptr_t ptr_address = ptr & BSG_PACStrippingMaskArm64e;
 */
#define BSG_PACStrippingMaskArm64e 0x0000000fffffffff

/** Remove any pointer tagging from an instruction address
 * On armv7 the least significant bit of the pointer distinguishes
 * between thumb mode (2-byte instructions) and normal mode (4-byte
 * instructions). On arm64 all instructions are 4-bytes wide so the two least
 * significant bytes should always be 0. On x86_64 and i386, instructions are
 * variable length so all bits are signficant.
 */
#if defined(__arm__)
#define DETAG_INSTRUCTION_ADDRESS(A) ((A) & ~(1UL))
#elif defined(__arm64__)
#define DETAG_INSTRUCTION_ADDRESS(A) ((A) & ~(3UL))
#else
#define DETAG_INSTRUCTION_ADDRESS(A) (A)
#endif

/** Step backwards by one instruction.
 * The backtrace of an objective-C program is expected to contain return
 * addresses not call instructions, as that is what can easily be read from
 * the stack. This is not a problem except for a few cases where the return
 * address is inside a different symbol than the call address.
 */
#define CALL_INSTRUCTION_FROM_RETURN_ADDRESS(A)                                \
    (DETAG_INSTRUCTION_ADDRESS((A)) - 1)

/** Represents an entry in a frame list.
 * This is modeled after the various i386/x64 frame walkers in the xnu source,
 * and seems to work fine in ARM as well. I haven't included the args pointer
 * since it's not needed in this context.
 */
typedef struct BSG_KSFrameEntry {
    /** The previous frame in the list. */
    const struct BSG_KSFrameEntry *const previous;

    /** The instruction address. */
    const uintptr_t return_address;
} BSG_KSFrameEntry;

// Avoiding static functions due to linker issues.

int bsg_ksbt_backtraceLength(
    const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    const uintptr_t instructionAddress =
        bsg_ksmachinstructionAddress(machineContext);

    if (instructionAddress == 0) {
        return 0;
    }

    BSG_KSFrameEntry frame = {0};
    const uintptr_t framePtr = bsg_ksmachframePointer(machineContext);
    if (framePtr == 0 || bsg_ksmachcopyMem((void *)framePtr, &frame,
                                           sizeof(frame)) != KERN_SUCCESS) {
        return 1;
    }
    for (int i = 1; i < BSG_kBacktraceGiveUpPoint; i++) {
        if (frame.previous == 0 ||
            bsg_ksmachcopyMem(frame.previous, &frame, sizeof(frame)) !=
                KERN_SUCCESS) {
            return i;
        }
    }

    return BSG_kBacktraceGiveUpPoint;
}

bool bsg_ksbt_isBacktraceTooLong(
    const BSG_STRUCT_MCONTEXT_L *const machineContext, int maxLength) {
    const uintptr_t instructionAddress =
        bsg_ksmachinstructionAddress(machineContext);

    if (instructionAddress == 0) {
        return 0;
    }

    BSG_KSFrameEntry frame = {0};
    const uintptr_t framePtr = bsg_ksmachframePointer(machineContext);
    if (framePtr == 0 || bsg_ksmachcopyMem((void *)framePtr, &frame,
                                           sizeof(frame)) != KERN_SUCCESS) {
        return 1;
    }
    for (int i = 1; i < maxLength; i++) {
        if (frame.previous == 0 ||
            bsg_ksmachcopyMem(frame.previous, &frame, sizeof(frame)) !=
                KERN_SUCCESS) {
            return false;
        }
    }

    return true;
}

int bsg_ksbt_backtraceThreadState(
    const BSG_STRUCT_MCONTEXT_L *const machineContext,
    uintptr_t *const backtraceBuffer, const int skipEntries,
    const int maxEntries) {
    if (maxEntries == 0) {
        return 0;
    }

    int i = 0;

    if (skipEntries == 0) {
        const uintptr_t instructionAddress =
            bsg_ksmachinstructionAddress(machineContext);
        backtraceBuffer[i] = instructionAddress;
        i++;

        if (i == maxEntries) {
            return i;
        }
    }

    if (skipEntries <= 1) {
        uintptr_t linkRegister = bsg_ksmachlinkRegister(machineContext);

        if (linkRegister) {
            backtraceBuffer[i] = linkRegister;
            i++;

            if (i == maxEntries) {
                return i;
            }
        }
    }

    BSG_KSFrameEntry frame = {0};

    const uintptr_t framePtr = bsg_ksmachframePointer(machineContext);
    if (framePtr == 0 || bsg_ksmachcopyMem((void *)framePtr, &frame,
                                           sizeof(frame)) != KERN_SUCCESS) {
        return 0;
    }
    for (int j = 1; j < skipEntries; j++) {
        if (frame.previous == 0 ||
            bsg_ksmachcopyMem(frame.previous, &frame, sizeof(frame)) !=
                KERN_SUCCESS) {
            return 0;
        }
    }

    for (; i < maxEntries; i++) {
#if defined(__arm64__)
        // Strip program auth code from address prior to storing address.
        // Intended for Arm64e but is a no-op on other Arm64 archs.
        backtraceBuffer[i] = frame.return_address & BSG_PACStrippingMaskArm64e;
#else
        backtraceBuffer[i] = frame.return_address;
#endif
        if (backtraceBuffer[i] == 0 || frame.previous == 0 ||
            bsg_ksmachcopyMem(frame.previous, &frame, sizeof(frame)) !=
                KERN_SUCCESS) {
            break;
        }
    }
    return i;
}

int bsg_ksbt_backtraceThread(const thread_t thread,
                             uintptr_t *const backtraceBuffer,
                             const int maxEntries) {
    BSG_STRUCT_MCONTEXT_L machineContext;

    if (!bsg_ksmachthreadState(thread, &machineContext)) {
        return 0;
    }

    return bsg_ksbt_backtraceThreadState(&machineContext, backtraceBuffer, 0,
                                         maxEntries);
}

int bsg_ksbt_backtracePthread(const pthread_t thread,
                              uintptr_t *const backtraceBuffer,
                              const int maxEntries) {
    const thread_t mach_thread = bsg_ksmachmachThreadFromPThread(thread);
    if (mach_thread == 0) {
        return 0;
    }
    return bsg_ksbt_backtraceThread(mach_thread, backtraceBuffer, maxEntries);
}

int bsg_ksbt_backtraceSelf(uintptr_t *const backtraceBuffer,
                           const int maxEntries) {
    return bsg_ksbt_backtraceThread(bsg_ksmachthread_self(), backtraceBuffer,
                                    maxEntries);
}

void bsg_ksbt_symbolicate(const uintptr_t *const backtraceBuffer,
                          Dl_info *const symbolsBuffer, const int numEntries,
                          const int skippedEntries) {
    int i = 0;

    if (!skippedEntries && i < numEntries) {
        bsg_ksdldladdr(backtraceBuffer[i], &symbolsBuffer[i]);
        i++;
    }

    for (; i < numEntries; i++) {
        bsg_ksdldladdr(CALL_INSTRUCTION_FROM_RETURN_ADDRESS(backtraceBuffer[i]),
                       &symbolsBuffer[i]);
    }
}
