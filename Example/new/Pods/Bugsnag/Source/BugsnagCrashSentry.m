//
//  BugsnagCrashSentry.m
//  Pods
//
//  Created by Jamie Lynch on 11/08/2017.
//
//

#import "BSG_KSCrashAdvanced.h"
#import "BSG_KSCrashC.h"

#import "BugsnagCrashSentry.h"
#import "BugsnagLogger.h"
#import "BugsnagSink.h"

NSUInteger const BSG_MAX_STORED_REPORTS = 12;

@implementation BugsnagCrashSentry

- (void)install:(BugsnagConfiguration *)config
      apiClient:(BugsnagErrorReportApiClient *)apiClient
        onCrash:(BSGReportCallback)onCrash {

    BugsnagSink *sink = [[BugsnagSink alloc] initWithApiClient:apiClient];
    [BSG_KSCrash sharedInstance].sink = sink;
    [BSG_KSCrash sharedInstance].introspectMemory = YES;
    [BSG_KSCrash sharedInstance].deleteBehaviorAfterSendAll =
        BSG_KSCDeleteOnSucess;
    [BSG_KSCrash sharedInstance].onCrash = onCrash;
    [BSG_KSCrash sharedInstance].maxStoredReports = BSG_MAX_STORED_REPORTS;
    [BSG_KSCrash sharedInstance].demangleLanguages = 0;

    if (!config.autoNotify) {
        bsg_kscrash_setHandlingCrashTypes(BSG_KSCrashTypeUserReported);
    }
    if (![[BSG_KSCrash sharedInstance] install]) {
        bsg_log_err(@"Failed to install crash handler. No exceptions will be "
                    @"reported!");
    }

    [sink.apiClient flushPendingData];
}

- (void)reportUserException:(NSString *)reportName
                     reason:(NSString *)reportMessage {

    [[BSG_KSCrash sharedInstance] reportUserException:reportName
                                               reason:reportMessage
                                             language:NULL
                                           lineOfCode:@""
                                           stackTrace:@[]
                                     terminateProgram:NO];
}

@end
