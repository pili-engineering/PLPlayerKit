//
//  BugsnagSessionTrackingPayload.h
//  Bugsnag
//
//  Created by Jamie Lynch on 27/11/2017.
//  Copyright © 2017 Bugsnag. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "BugsnagSession.h"

@interface BugsnagSessionTrackingPayload : NSObject

- (instancetype)initWithSessions:(NSArray<BugsnagSession *> *)sessions;

- (NSDictionary *)toJson;

@property NSArray<BugsnagSession *> *sessions;

@end
