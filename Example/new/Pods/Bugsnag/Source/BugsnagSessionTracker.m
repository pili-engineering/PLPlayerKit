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

@interface BugsnagSessionTracker ()
@property BugsnagConfiguration *config;
@property BugsnagSessionFileStore *sessionStore;
@property BugsnagSessionTrackingApiClient *apiClient;
@property BOOL trackedFirstSession;
@end

@implementation BugsnagSessionTracker

- (instancetype)initWithConfig:(BugsnagConfiguration *)config
                     apiClient:(BugsnagSessionTrackingApiClient *)apiClient
                      callback:(void(^)(BugsnagSession *))callback {
    if (self = [super init]) {
        _config = config;
        _apiClient = apiClient;
        _callback = callback;

        NSString *bundleName = [[NSBundle mainBundle] infoDictionary][@"CFBundleName"];
        NSString *storePath = [BugsnagFileStore findReportStorePath:@"Sessions"
                                                         bundleName:bundleName];
        if (!storePath) {
            BSG_KSLOG_ERROR(@"Failed to initialize session store.");
        }
        _sessionStore = [BugsnagSessionFileStore storeWithPath:storePath];
    }
    return self;
}

- (void)startNewSession:(NSDate *)date
               withUser:(BugsnagUser *)user
           autoCaptured:(BOOL)autoCaptured {

    _currentSession = [[BugsnagSession alloc] initWithId:[[NSUUID UUID] UUIDString]
                                                startDate:date
                                                     user:user
                                             autoCaptured:autoCaptured];

    if ((self.config.shouldAutoCaptureSessions || !autoCaptured) && [self.config shouldSendReports]) {
        [self trackSession];
    }
    _isInForeground = YES;
}

- (void)trackSession {
    [self.sessionStore write:self.currentSession];
    self.trackedFirstSession = YES;
    
    if (self.callback) {
        self.callback(self.currentSession);
    }
}

- (void)onAutoCaptureEnabled {
    if (!self.trackedFirstSession) {
        if (self.currentSession == nil) { // unlikely case, will be initialised later
            return;
        }
        [self trackSession];
    }
}

- (void)suspendCurrentSession:(NSDate *)date {
    _isInForeground = NO;
}

- (void)incrementHandledError {
    @synchronized (self.currentSession) {
        self.currentSession.handledCount++;
        if (self.callback && (self.config.shouldAutoCaptureSessions || !self.currentSession.autoCaptured)) {
            self.callback(self.currentSession);
        }
    }
}

- (void)send {
    @synchronized (self.sessionStore) {
        NSMutableArray *sessions = [NSMutableArray new];
        NSArray *fileIds = [self.sessionStore fileIds];

        for (NSDictionary *dict in [self.sessionStore allFiles]) {
            [sessions addObject:[[BugsnagSession alloc] initWithDictionary:dict]];
        }
        BugsnagSessionTrackingPayload *payload = [[BugsnagSessionTrackingPayload alloc] initWithSessions:sessions];

        if (payload.sessions.count > 0) {
            [self.apiClient sendData:payload
                         withPayload:[payload toJson]
                               toURL:self.config.sessionURL
                             headers:self.config.sessionApiHeaders
                        onCompletion:^(id data, BOOL success, NSError *error) {

                            if (success && error == nil) {
                                NSLog(@"Sent sessions to Bugsnag");

                                for (NSString *fileId in fileIds) {
                                    [self.sessionStore deleteFileWithId:fileId];
                                }
                            } else {
                                NSLog(@"Failed to send sessions to Bugsnag: %@", error);
                            }
                        }];
        }
    }
}

@end
