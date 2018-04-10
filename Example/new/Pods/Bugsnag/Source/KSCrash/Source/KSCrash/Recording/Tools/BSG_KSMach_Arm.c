//
//  KSMach_Arm.c
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

#if defined(__arm__)

#include "BSG_KSMach.h"

//#define BSG_KSLogger_LocalLevel TRACE
#include "BSG_KSLogger.h"

static const char *bsg_g_registerNames[] = {
    "r0", "r1",  "r2",  "r3", "r4", "r5", "r6", "r7",  "r8",
    "r9", "r10", "r11", "ip", "sp", "lr", "pc", "cpsr"};
static const int bsg_g_registerNamesCount =
    sizeof(bsg_g_registerNames) / sizeof(*bsg_g_registerNames);

static const char *bsg_g_exceptionRegisterNames[] = {"exception", "fsr", "far"};
static const int bsg_g_exceptionRegisterNamesCount =
    sizeof(bsg_g_exceptionRegisterNames) /
    sizeof(*bsg_g_exceptionRegisterNames);

uintptr_t
bsg_ksmachframePointer(const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    return machineContext->__ss.__r[7];
}

uintptr_t
bsg_ksmachstackPointer(const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    return machineContext->__ss.__sp;
}

uintptr_t bsg_ksmachinstructionAddress(
    const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    return machineContext->__ss.__pc;
}

uintptr_t
bsg_ksmachlinkRegister(const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    return machineContext->__ss.__lr;
}

bool bsg_ksmachthreadState(const thread_t thread,
                           BSG_STRUCT_MCONTEXT_L *const machineContext) {
    return bsg_ksmachfillState(thread, (thread_state_t)&machineContext->__ss,
                               ARM_THREAD_STATE, ARM_THREAD_STATE_COUNT);
}

bool bsg_ksmachfloatState(const thread_t thread,
                          BSG_STRUCT_MCONTEXT_L *const machineContext) {
    return bsg_ksmachfillState(thread, (thread_state_t)&machineContext->__fs,
                               ARM_VFP_STATE, ARM_VFP_STATE_COUNT);
}

bool bsg_ksmachexceptionState(const thread_t thread,
                              BSG_STRUCT_MCONTEXT_L *const machineContext) {
    return bsg_ksmachfillState(thread, (thread_state_t)&machineContext->__es,
                               ARM_EXCEPTION_STATE, ARM_EXCEPTION_STATE_COUNT);
}

int bsg_ksmachnumRegisters(void) { return bsg_g_registerNamesCount; }

const char *bsg_ksmachregisterName(const int regNumber) {
    if (regNumber < bsg_ksmachnumRegisters()) {
        return bsg_g_registerNames[regNumber];
    }
    return NULL;
}

uint64_t
bsg_ksmachregisterValue(const BSG_STRUCT_MCONTEXT_L *const machineContext,
                        const int regNumber) {
    if (regNumber <= 12) {
        return machineContext->__ss.__r[regNumber];
    }

    switch (regNumber) {
    case 13:
        return machineContext->__ss.__sp;
    case 14:
        return machineContext->__ss.__lr;
    case 15:
        return machineContext->__ss.__pc;
    case 16:
        return machineContext->__ss.__cpsr;
    }

    BSG_KSLOG_ERROR("Invalid register number: %d", regNumber);
    return 0;
}

int bsg_ksmachnumExceptionRegisters(void) {
    return bsg_g_exceptionRegisterNamesCount;
}

const char *bsg_ksmachexceptionRegisterName(const int regNumber) {
    if (regNumber < bsg_ksmachnumExceptionRegisters()) {
        return bsg_g_exceptionRegisterNames[regNumber];
    }
    BSG_KSLOG_ERROR("Invalid register number: %d", regNumber);
    return NULL;
}

uint64_t bsg_ksmachexceptionRegisterValue(
    const BSG_STRUCT_MCONTEXT_L *const machineContext, const int regNumber) {
    switch (regNumber) {
    case 0:
        return machineContext->__es.__exception;
    case 1:
        return machineContext->__es.__fsr;
    case 2:
        return machineContext->__es.__far;
    }

    BSG_KSLOG_ERROR("Invalid register number: %d", regNumber);
    return 0;
}

uintptr_t
bsg_ksmachfaultAddress(const BSG_STRUCT_MCONTEXT_L *const machineContext) {
    return machineContext->__es.__far;
}

int bsg_ksmachstackGrowDirection(void) { return -1; }

#endif
