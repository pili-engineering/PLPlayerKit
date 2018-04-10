//
//  BugsnagErrorReportApiClient.m
//  Pods
//
//  Created by Jamie Lynch on 11/08/2017.
//
//

#import "BugsnagErrorReportApiClient.h"
#import "Bugsnag.h"
#import "BugsnagLogger.h"
#import "BugsnagNotifier.h"
#import "BugsnagSink.h"
#import "BugsnagKeys.h"

@interface BSGDeliveryOperation : NSOperation
@end

@implementation BugsnagErrorReportApiClient

- (NSOperation *)deliveryOperation {
    return [BSGDeliveryOperation new];
}

@end

@implementation BSGDeliveryOperation

- (void)main {
    @autoreleasepool {
        @try {
            [[BSG_KSCrash sharedInstance]
                    sendAllReportsWithCompletion:^(NSArray *filteredReports,
                            BOOL completed, NSError *error) {
                        if (error) {
                            bsg_log_warn(@"Failed to send reports: %@", error);
                        } else if (filteredReports.count > 0) {
                            bsg_log_info(@"Reports sent.");
                        }
                    }];
        } @catch (NSException *e) {
            bsg_log_err(@"Could not send report: %@", e);
        }
    }
}
@end
