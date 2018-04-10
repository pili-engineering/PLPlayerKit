//
// Created by Jamie Lynch on 30/11/2017.
// Copyright (c) 2017 Bugsnag. All rights reserved.
//

#import "BugsnagSessionTrackingApiClient.h"
#import "BugsnagConfiguration.h"
#import "BugsnagSessionTrackingPayload.h"
#import "BugsnagLogger.h"
#import "Bugsnag.h"
#import "BugsnagKeys.h"
#import "BugsnagSession.h"

@implementation BugsnagSessionTrackingApiClient

- (NSOperation *)deliveryOperation {
    return [NSOperation new];
}

@end
