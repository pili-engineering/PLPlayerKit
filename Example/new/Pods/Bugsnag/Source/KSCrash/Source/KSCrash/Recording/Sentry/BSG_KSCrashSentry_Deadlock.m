//
//  BSG_KSCrashSentry_Deadlock.m
//
//  Created by Karl Stenerud on 2012-12-09.
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

#import "BSG_KSCrashSentry_Deadlock.h"
#import "BSG_KSCrashSentry_Private.h"
#include "BSG_KSMach.h"

//#define BSG_KSLogger_LocalLevel TRACE
#import "BSG_KSLogger.h"

#define kIdleInterval 5.0f

@class BSG_KSCrashDeadlockMonitor;

// ============================================================================
#pragma mark - Globals -
// ============================================================================

/** Flag noting if we've installed our custom handlers or not.
 * It's not fully thread safe, but it's safer than locking and slightly better
 * than nothing.
 */
static volatile sig_atomic_t bsg_g_installed = 0;

/** Thread which monitors other threads. */
static BSG_KSCrashDeadlockMonitor *bsg_g_monitor;

/** Context to fill with crash information. */
static BSG_KSCrash_SentryContext *bsg_g_context;

/** Interval between watchdog pulses. */
static NSTimeInterval bsg_g_watchdogInterval = 0;

// ============================================================================
#pragma mark - X -
// ============================================================================

@interface BSG_KSCrashDeadlockMonitor : NSObject

@property(nonatomic, readwrite, retain) NSThread *monitorThread;
@property(nonatomic, readwrite, assign) thread_t mainThread;
@property(atomic, readwrite, assign) BOOL awaitingResponse;

@end

@implementation BSG_KSCrashDeadlockMonitor

@synthesize monitorThread = _monitorThread;
@synthesize mainThread = _mainThread;
@synthesize awaitingResponse = _awaitingResponse;

- (id)init {
    if ((self = [super init])) {
        // target (self) is retained until selector (runMonitor) exits.
        self.monitorThread =
            [[NSThread alloc] initWithTarget:self
                                    selector:@selector(runMonitor)
                                      object:nil];
        self.monitorThread.name = @"KSCrash Deadlock Detection Thread";
        [self.monitorThread start];

        dispatch_async(dispatch_get_main_queue(), ^{
          self.mainThread = bsg_ksmachthread_self();
        });
    }
    return self;
}

- (void)cancel {
    [self.monitorThread cancel];
}

- (void)watchdogPulse {
    __block id blockSelf = self;
    self.awaitingResponse = YES;
    dispatch_async(dispatch_get_main_queue(), ^{
      [blockSelf watchdogAnswer];
    });
}

- (void)watchdogAnswer {
    self.awaitingResponse = NO;
}

- (void)handleDeadlock {
    bsg_kscrashsentry_beginHandlingCrash(bsg_g_context);

    BSG_KSLOG_DEBUG(@"Filling out context.");
    bsg_g_context->crashType = BSG_KSCrashTypeMainThreadDeadlock;
    bsg_g_context->offendingThread = self.mainThread;
    bsg_g_context->registersAreValid = false;

    BSG_KSLOG_DEBUG(@"Calling main crash handler.");
    bsg_g_context->onCrash();

    BSG_KSLOG_DEBUG(@"Crash handling complete. Restoring original handlers.");
    bsg_kscrashsentry_uninstall(BSG_KSCrashTypeAll);

    BSG_KSLOG_DEBUG(@"Calling abort()");
    abort();
}

- (void)runMonitor {
    BOOL cancelled = NO;
    do {
        // Only do a watchdog check if the watchdog interval is > 0.
        // If the interval is <= 0, just idle until the user changes it.
        @autoreleasepool {
            NSTimeInterval sleepInterval = bsg_g_watchdogInterval;
            BOOL runWatchdogCheck = sleepInterval > 0;
            if (!runWatchdogCheck) {
                sleepInterval = kIdleInterval;
            }
            [NSThread sleepForTimeInterval:sleepInterval];
            cancelled = self.monitorThread.isCancelled;
            if (!cancelled && runWatchdogCheck) {
                if (self.awaitingResponse) {
                    [self handleDeadlock];
                } else {
                    [self watchdogPulse];
                }
            }
        }
    } while (!cancelled);
}

@end

// ============================================================================
#pragma mark - API -
// ============================================================================

bool bsg_kscrashsentry_installDeadlockHandler(
    BSG_KSCrash_SentryContext *context) {
    BSG_KSLOG_DEBUG(@"Installing deadlock handler.");
    if (bsg_g_installed) {
        BSG_KSLOG_DEBUG(@"Deadlock handler already installed.");
        return true;
    }
    bsg_g_installed = 1;

    bsg_g_context = context;

    BSG_KSLOG_DEBUG(@"Creating new deadlock monitor.");
    bsg_g_monitor = [[BSG_KSCrashDeadlockMonitor alloc] init];
    return true;
}

void bsg_kscrashsentry_uninstallDeadlockHandler(void) {
    BSG_KSLOG_DEBUG(@"Uninstalling deadlock handler.");
    if (!bsg_g_installed) {
        BSG_KSLOG_DEBUG(@"Deadlock handler was already uninstalled.");
        return;
    }

    BSG_KSLOG_DEBUG(@"Stopping deadlock monitor.");
    [bsg_g_monitor cancel];
    bsg_g_monitor = nil;

    bsg_g_installed = 0;
}

void bsg_kscrashsentry_setDeadlockHandlerWatchdogInterval(double value) {
    bsg_g_watchdogInterval = value;
}
