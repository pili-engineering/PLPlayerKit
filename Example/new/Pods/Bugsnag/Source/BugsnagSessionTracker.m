//
//  BugsnagSessionTracker.m
//  Bugsnag
//
//  Created by Jamie Lynch on 24/11/2017.
//  Copyright © 2017 Bugsnag. All rights reserved.
//

#import "BugsnagSessionTracker.h"
#import "BugsnagSessionFileStore.h"
#import "BSG_KSLogger.h"
#import "BugsnagSessionTrackingPayload.h"
#import "BugsnagSessionTrackingApiClient.h"
#import "BugsnagLogger.h"

/**
 Number of seconds in background required to make a new session
 */
NSTimeInterval const BSGNewSessionBackgroundDuration = 60;

@interface BugsnagSessionTracker ()
@property (weak, nonatomic) BugsnagConfiguration *config;
@property (strong, nonatomic) BugsnagSessionFileStore *sessionStore;
@property (strong, nonatomic) BugsnagSessionTrackingApiClient *apiClient;
@property (strong, nonatomic) NSDate *backgroundStartTime;

@property (strong, readwrite) BugsnagSession *currentSession;

/**
 * Called when a session is altered
 */
@property (nonatomic, strong, readonly) SessionTrackerCallback callback;
@end

@implementation BugsnagSessionTracker

- (instancetype)initWithConfig:(BugsnagConfiguration *)config
            postRecordCallback:(void(^)(BugsnagSession *))callback {
    if (self = [super init]) {
        _config = config;
        _apiClient = [[BugsnagSessionTrackingApiClient alloc] initWithConfig:config queueName:@"Session API queue"];
        _callback = callback;

        NSString *storePath = [BugsnagFileStore findReportStorePath:@"Sessions"];
        if (!storePath) {
            BSG_KSLOG_ERROR(@"Failed to initialize session store.");
        }
        _sessionStore = [BugsnagSessionFileStore storeWithPath:storePath];
    }
    return self;
}

#pragma mark - Creating and sending a new session

- (void)startNewSession {
    [self startNewSessionWithAutoCaptureValue:NO];
}

- (void)startNewSessionIfAutoCaptureEnabled {
    if (self.config.shouldAutoCaptureSessions  && [self.config shouldSendReports]) {
        [self startNewSessionWithAutoCaptureValue:YES];
    }
}

- (void)startNewSessionWithAutoCaptureValue:(BOOL)isAutoCaptured {
    if (self.config.sessionURL == nil) {
        bsg_log_err(@"The session tracking endpoint has not been set. Session tracking is disabled");
        return;
    }
    self.currentSession = [[BugsnagSession alloc] initWithId:[[NSUUID UUID] UUIDString]
                                                   startDate:[NSDate date]
                                                        user:self.config.currentUser
                                                autoCaptured:isAutoCaptured];

    [self.sessionStore write:self.currentSession];

    if (self.callback) {
        self.callback(self.currentSession);
    }
    [self.apiClient deliverSessionsInStore:self.sessionStore];
}

#pragma mark - Handling events

- (void)handleAppBackgroundEvent {
    self.backgroundStartTime = [NSDate date];
}

- (void)handleAppForegroundEvent {
    if (self.backgroundStartTime
        && [[NSDate date] timeIntervalSinceDate:self.backgroundStartTime] >= BSGNewSessionBackgroundDuration) {
        [self startNewSessionIfAutoCaptureEnabled];
    }
    self.backgroundStartTime = nil;
}

- (void)handleHandledErrorEvent {
    if (self.currentSession == nil) {
        return;
    }

    @synchronized (self.currentSession) {
        self.currentSession.handledCount++;
        if (self.callback && (self.config.shouldAutoCaptureSessions || !self.currentSession.autoCaptured)) {
            self.callback(self.currentSession);
        }
    }
}

@end
