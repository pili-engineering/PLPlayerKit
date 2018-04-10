//
//  BSG_KSCrashType.c
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

#include "BSG_KSCrashType.h"

#include <stdlib.h>

static const struct {
    const BSG_KSCrashType type;
    const char *const name;
} bsg_g_crashTypes[] = {
#define BSG_CRASHTYPE(NAME)                                                    \
    { NAME, #NAME }
    BSG_CRASHTYPE(BSG_KSCrashTypeMachException),
    BSG_CRASHTYPE(BSG_KSCrashTypeSignal),
    BSG_CRASHTYPE(BSG_KSCrashTypeCPPException),
    BSG_CRASHTYPE(BSG_KSCrashTypeNSException),
    BSG_CRASHTYPE(BSG_KSCrashTypeMainThreadDeadlock),
    BSG_CRASHTYPE(BSG_KSCrashTypeUserReported),
};
static const int bsg_g_crashTypesCount =
    sizeof(bsg_g_crashTypes) / sizeof(*bsg_g_crashTypes);

const char *bsg_kscrashtype_name(const BSG_KSCrashType crashType) {
    for (int i = 0; i < bsg_g_crashTypesCount; i++) {
        if (bsg_g_crashTypes[i].type == crashType) {
            return bsg_g_crashTypes[i].name;
        }
    }
    return NULL;
}
