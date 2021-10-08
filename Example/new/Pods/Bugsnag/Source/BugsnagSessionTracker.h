//
//  BugsnagSessionTracker.h
//  Bugsnag
//
//  Created by Jamie Lynch on 24/11/2017.
//  Copyright © 2017 Bugsnag. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "BugsnagSession.h"
#import "BugsnagConfiguration.h"

@class BugsnagSessionTrackingApiClient;

typedef void (^SessionTrackerCallback)(BugsnagSession *newSession);

@interface BugsnagSessionTracker : NSObject

- (instancetype)initWithConfig:(BugsnagConfiguration *)config
                     apiClient:(BugsnagSessionTrackingApiClient *)apiClient
                      callback:(void(^)(BugsnagSession *))callback;

- (void)startNewSession:(NSDate *)date
               withUser:(BugsnagUser *)user
           autoCaptured:(BOOL)autoCaptured;

- (void)suspendCurrentSession:(NSDate *)date;
- (void)incrementHandledError;
- (void)send;
- (void)onAutoCaptureEnabled;

@property (readonly) BugsnagSession *currentSession;
@property (readonly) BOOL isInForeground;

/**
 * Called when a session is altered
 */
@property SessionTrackerCallback callback;

@end
