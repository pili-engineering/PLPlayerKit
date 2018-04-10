//
//  BSG_KSCrashState.c
//
//  Created by Karl Stenerud on 2012-02-05.
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

#include "BSG_KSCrashState.h"

#include "BSG_KSFileUtils.h"
#include "BSG_KSJSONCodec.h"
#include "BSG_KSMach.h"

//#define BSG_KSLogger_LocalLevel TRACE
#include "BSG_KSLogger.h"

#include <errno.h>
#include <fcntl.h>
#include <mach/mach_time.h>
#include <stdlib.h>
#include <unistd.h>

// ============================================================================
#pragma mark - Constants -
// ============================================================================

#define BSG_kFormatVersion 1

#define BSG_kKeyFormatVersion "version"
#define BSG_kKeyCrashedLastLaunch "crashedLastLaunch"
#define BSG_kKeyActiveDurationSinceLastCrash "activeDurationSinceLastCrash"
#define BSG_kKeyBackgroundDurationSinceLastCrash                               \
    "backgroundDurationSinceLastCrash"
#define BSG_kKeyLaunchesSinceLastCrash "launchesSinceLastCrash"
#define BSG_kKeySessionsSinceLastCrash "sessionsSinceLastCrash"
#define BSG_kKeySessionsSinceLaunch "sessionsSinceLaunch"

// ============================================================================
#pragma mark - Globals -
// ============================================================================

/** Location where stat file is stored. */
static const char *bsg_g_stateFilePath;

/** Current state. */
static BSG_KSCrash_State *bsg_g_state;

// Avoiding static functions due to linker issues.

// ============================================================================
#pragma mark - JSON Encoding -
// ============================================================================

int bsg_kscrashstate_i_onBooleanElement(const char *const name,
                                        const bool value,
                                        void *const userData) {
    BSG_KSCrash_State *state = userData;

    if (strcmp(name, BSG_kKeyCrashedLastLaunch) == 0) {
        state->crashedLastLaunch = value;
    }

    return BSG_KSJSON_OK;
}

int bsg_kscrashstate_i_onFloatingPointElement(const char *const name,
                                              const double value,
                                              void *const userData) {
    BSG_KSCrash_State *state = userData;

    if (strcmp(name, BSG_kKeyActiveDurationSinceLastCrash) == 0) {
        state->activeDurationSinceLastCrash = value;
    }
    if (strcmp(name, BSG_kKeyBackgroundDurationSinceLastCrash) == 0) {
        state->backgroundDurationSinceLastCrash = value;
    }

    return BSG_KSJSON_OK;
}

int bsg_kscrashstate_i_onIntegerElement(const char *const name,
                                        const long long value,
                                        void *const userData) {
    BSG_KSCrash_State *state = userData;

    if (strcmp(name, BSG_kKeyFormatVersion) == 0) {
        if (value != BSG_kFormatVersion) {
            BSG_KSLOG_ERROR("Expected version 1 but got %lld", value);
            return BSG_KSJSON_ERROR_INVALID_DATA;
        }
    } else if (strcmp(name, BSG_kKeyLaunchesSinceLastCrash) == 0) {
        state->launchesSinceLastCrash = (int)value;
    } else if (strcmp(name, BSG_kKeySessionsSinceLastCrash) == 0) {
        state->sessionsSinceLastCrash = (int)value;
    }

    // FP value might have been written as a whole number.
    return bsg_kscrashstate_i_onFloatingPointElement(name, value, userData);
}

int bsg_kscrashstate_i_onNullElement(__unused const char *const name,
                                     __unused void *const userData) {
    return BSG_KSJSON_OK;
}

int bsg_kscrashstate_i_onStringElement(__unused const char *const name,
                                       __unused const char *const value,
                                       __unused void *const userData) {
    return BSG_KSJSON_OK;
}

int bsg_kscrashstate_i_onBeginObject(__unused const char *const name,
                                     __unused void *const userData) {
    return BSG_KSJSON_OK;
}

int bsg_kscrashstate_i_onBeginArray(__unused const char *const name,
                                    __unused void *const userData) {
    return BSG_KSJSON_OK;
}

int bsg_kscrashstate_i_onEndContainer(__unused void *const userData) {
    return BSG_KSJSON_OK;
}

int bsg_kscrashstate_i_onEndData(__unused void *const userData) {
    return BSG_KSJSON_OK;
}

/** Callback for adding JSON data.
 */
int bsg_kscrashstate_i_addJSONData(const char *const data, const size_t length,
                                   void *const userData) {
    const int fd = *((int *)userData);
    const bool success = bsg_ksfuwriteBytesToFD(fd, data, (ssize_t)length);
    return success ? BSG_KSJSON_OK : BSG_KSJSON_ERROR_CANNOT_ADD_DATA;
}

// ============================================================================
#pragma mark - Utility -
// ============================================================================

/** Load the persistent state portion of a crash context.
 *
 * @param context The context to load into.
 *
 * @param path The path to the file to read.
 *
 * @return true if the operation was successful.
 */
bool bsg_kscrashstate_i_loadState(BSG_KSCrash_State *const context,
                                  const char *const path) {
    // Stop if the file doesn't exist.
    // This is expected on the first run of the app.
    const int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return false;
    }
    close(fd);

    char *data;
    size_t length;
    if (!bsg_ksfureadEntireFile(path, &data, &length)) {
        BSG_KSLOG_ERROR("%s: Could not load file", path);
        return false;
    }

    BSG_KSJSONDecodeCallbacks callbacks;
    callbacks.onBeginArray = bsg_kscrashstate_i_onBeginArray;
    callbacks.onBeginObject = bsg_kscrashstate_i_onBeginObject;
    callbacks.onBooleanElement = bsg_kscrashstate_i_onBooleanElement;
    callbacks.onEndContainer = bsg_kscrashstate_i_onEndContainer;
    callbacks.onEndData = bsg_kscrashstate_i_onEndData;
    callbacks.onFloatingPointElement =
        bsg_kscrashstate_i_onFloatingPointElement;
    callbacks.onIntegerElement = bsg_kscrashstate_i_onIntegerElement;
    callbacks.onNullElement = bsg_kscrashstate_i_onNullElement;
    callbacks.onStringElement = bsg_kscrashstate_i_onStringElement;

    size_t errorOffset = 0;

    const int result =
        bsg_ksjsondecode(data, length, &callbacks, context, &errorOffset);
    free(data);
    if (result != BSG_KSJSON_OK) {
        BSG_KSLOG_ERROR("%s, offset %d: %s", path, errorOffset,
                        bsg_ksjsonstringForError(result));
        return false;
    }
    return true;
}

/** Save the persistent state portion of a crash context.
 *
 * @param state The context to save from.
 *
 * @param path The path to the file to create.
 *
 * @return true if the operation was successful.
 */
bool bsg_kscrashstate_i_saveState(const BSG_KSCrash_State *const state,
                                  const char *const path) {
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        BSG_KSLOG_ERROR("Could not open file %s for writing: %s", path,
                        strerror(errno));
        return false;
    }

    BSG_KSJSONEncodeContext JSONContext;
    bsg_ksjsonbeginEncode(&JSONContext, true, bsg_kscrashstate_i_addJSONData,
                          &fd);

    int result;
    if ((result = bsg_ksjsonbeginObject(&JSONContext, NULL)) != BSG_KSJSON_OK) {
        goto done;
    }
    if ((result = bsg_ksjsonaddIntegerElement(
             &JSONContext, BSG_kKeyFormatVersion, BSG_kFormatVersion)) !=
        BSG_KSJSON_OK) {
        goto done;
    }
    // Record this launch crashed state into "crashed last launch" field.
    if ((result = bsg_ksjsonaddBooleanElement(
             &JSONContext, BSG_kKeyCrashedLastLaunch,
             state->crashedThisLaunch)) != BSG_KSJSON_OK) {
        goto done;
    }
    if ((result = bsg_ksjsonaddFloatingPointElement(
             &JSONContext, BSG_kKeyActiveDurationSinceLastCrash,
             state->activeDurationSinceLastCrash)) != BSG_KSJSON_OK) {
        goto done;
    }
    if ((result = bsg_ksjsonaddFloatingPointElement(
             &JSONContext, BSG_kKeyBackgroundDurationSinceLastCrash,
             state->backgroundDurationSinceLastCrash)) != BSG_KSJSON_OK) {
        goto done;
    }
    if ((result = bsg_ksjsonaddIntegerElement(
             &JSONContext, BSG_kKeyLaunchesSinceLastCrash,
             state->launchesSinceLastCrash)) != BSG_KSJSON_OK) {
        goto done;
    }
    if ((result = bsg_ksjsonaddIntegerElement(
             &JSONContext, BSG_kKeySessionsSinceLastCrash,
             state->sessionsSinceLastCrash)) != BSG_KSJSON_OK) {
        goto done;
    }
    result = bsg_ksjsonendEncode(&JSONContext);
    if (!bsg_ksfuflushWriteBuffer(fd)) {
        BSG_KSLOG_ERROR("Failed to flush write buffer");
    }

done:
    close(fd);

    if (result != BSG_KSJSON_OK) {
        BSG_KSLOG_ERROR("%s: %s", path, bsg_ksjsonstringForError(result));
        return false;
    }
    return true;
}

// ============================================================================
#pragma mark - API -
// ============================================================================

bool bsg_kscrashstate_init(const char *const stateFilePath,
                           BSG_KSCrash_State *const state) {
    bsg_g_stateFilePath = stateFilePath;
    bsg_g_state = state;

    bsg_kscrashstate_i_loadState(state, stateFilePath);

    state->sessionsSinceLaunch = 1;
    state->activeDurationSinceLaunch = 0;
    state->backgroundDurationSinceLaunch = 0;
    if (state->crashedLastLaunch) {
        state->activeDurationSinceLastCrash = 0;
        state->backgroundDurationSinceLastCrash = 0;
        state->launchesSinceLastCrash = 0;
        state->sessionsSinceLastCrash = 0;
    }
    state->crashedThisLaunch = false;

    // Simulate first transition to foreground
    state->launchesSinceLastCrash++;
    state->sessionsSinceLastCrash++;
    state->applicationIsInForeground = true;

    return bsg_kscrashstate_i_saveState(state, stateFilePath);
}

void bsg_kscrashstate_notifyAppActive(const bool isActive) {
    BSG_KSCrash_State *const state = bsg_g_state;

    state->applicationIsActive = isActive;
    if (isActive) {
        state->appStateTransitionTime = mach_absolute_time();
    } else {
        double duration = bsg_ksmachtimeDifferenceInSeconds(
            mach_absolute_time(), state->appStateTransitionTime);
        state->activeDurationSinceLaunch += duration;
        state->activeDurationSinceLastCrash += duration;
    }
}

void bsg_kscrashstate_notifyAppInForeground(const bool isInForeground) {
    BSG_KSCrash_State *const state = bsg_g_state;
    const char *const stateFilePath = bsg_g_stateFilePath;

    state->applicationIsInForeground = isInForeground;
    if (isInForeground) {
        double duration = bsg_ksmachtimeDifferenceInSeconds(
            mach_absolute_time(), state->appStateTransitionTime);
        state->backgroundDurationSinceLaunch += duration;
        state->backgroundDurationSinceLastCrash += duration;
        state->sessionsSinceLastCrash++;
        state->sessionsSinceLaunch++;
    } else {
        state->appStateTransitionTime = mach_absolute_time();
        bsg_kscrashstate_i_saveState(state, stateFilePath);
    }
}

void bsg_kscrashstate_notifyAppTerminate(void) {
    BSG_KSCrash_State *const state = bsg_g_state;
    const char *const stateFilePath = bsg_g_stateFilePath;

    const double duration = bsg_ksmachtimeDifferenceInSeconds(
        mach_absolute_time(), state->appStateTransitionTime);
    state->backgroundDurationSinceLastCrash += duration;
    bsg_kscrashstate_i_saveState(state, stateFilePath);
}

void bsg_kscrashstate_notifyAppCrash(void) {
    BSG_KSCrash_State *const state = bsg_g_state;
    const char *const stateFilePath = bsg_g_stateFilePath;

    const double duration = bsg_ksmachtimeDifferenceInSeconds(
        mach_absolute_time(), state->appStateTransitionTime);
    if (state->applicationIsActive) {
        state->activeDurationSinceLaunch += duration;
        state->activeDurationSinceLastCrash += duration;
    } else if (!state->applicationIsInForeground) {
        state->backgroundDurationSinceLaunch += duration;
        state->backgroundDurationSinceLastCrash += duration;
    }
    state->crashedThisLaunch = true;
    bsg_kscrashstate_i_saveState(state, stateFilePath);
}

const BSG_KSCrash_State *const bsg_kscrashstate_currentState(void) {
    return bsg_g_state;
}
