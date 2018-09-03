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

/**
 Create a new session tracker

 @param config The Bugsnag configuration to use
 @param callback A callback invoked each time a new session is started
 @return A new session tracker
 */
- (instancetype)initWithConfig:(BugsnagConfiguration *)config
            postRecordCallback:(void(^)(BugsnagSession *))callback;

/**
 Record and send a new session
 */
- (void)startNewSession;

/**
 Record a new auto-captured session if neededed. Auto-captured sessions are only
 recorded and sent if -[BugsnagConfiguration shouldAutoCaptureSessions] is YES
 */
- (void)startNewSessionIfAutoCaptureEnabled;

/**
 Handle the app foregrounding event. If more than 30s has elapsed since being
 sent to the background, records a new session if session auto-capture is
 enabled.
 Must be called from the main thread.
 */
- (void)handleAppForegroundEvent;

/**
 Handle the app backgrounding event. Tracks time between foreground and
 background to determine when to automatically record a session.
 Must be called from the main thread.
 */
- (void)handleAppBackgroundEvent;

/**
 Handle some variation of Bugsnag.notify() being called.
 Increases the number of handled errors recorded for the current session, if
 a session exists.
 */
- (void)handleHandledErrorEvent;


@property (nonatomic, strong, readonly) BugsnagSession *currentSession;

@end
