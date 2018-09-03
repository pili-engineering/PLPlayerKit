//
// Created by Jamie Lynch on 30/11/2017.
// Copyright (c) 2017 Bugsnag. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "BugsnagApiClient.h"

@class BugsnagSessionFileStore;


@interface BugsnagSessionTrackingApiClient : BugsnagApiClient


/**
 Asynchronously delivers sessions written to the store

 @param store The store containing the sessions to deliver
 */
- (void)deliverSessionsInStore:(BugsnagSessionFileStore *)store;
@end
