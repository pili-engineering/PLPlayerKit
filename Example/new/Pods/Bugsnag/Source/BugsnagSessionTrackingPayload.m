//
//  BugsnagSessionTrackingPayload.m
//  Bugsnag
//
//  Created by Jamie Lynch on 27/11/2017.
//  Copyright © 2017 Bugsnag. All rights reserved.
//

#import "BugsnagSessionTrackingPayload.h"
#import "BugsnagCollections.h"
#import "BugsnagNotifier.h"
#import "Bugsnag.h"
#import "BugsnagKeys.h"
#import "BSG_KSSystemInfo.h"
#import "BugsnagKSCrashSysInfoParser.h"

@interface Bugsnag ()
+ (BugsnagNotifier *)notifier;
@end

@implementation BugsnagSessionTrackingPayload

- (instancetype)initWithSessions:(NSArray<BugsnagSession *> *)sessions {
    if (self = [super init]) {
        _sessions = sessions;
    }
    return self;
}


- (NSDictionary *)toJson {
    
    NSMutableDictionary *dict = [NSMutableDictionary new];
    NSMutableArray *sessionData = [NSMutableArray new];
    
    for (BugsnagSession *session in self.sessions) {
        [sessionData addObject:[session toJson]];
    }
    BSGDictInsertIfNotNil(dict, sessionData, @"sessions");
    BSGDictSetSafeObject(dict, [Bugsnag notifier].details, BSGKeyNotifier);
    
    NSDictionary *systemInfo = [BSG_KSSystemInfo systemInfo];
    BSGDictSetSafeObject(dict, BSGParseAppState(systemInfo, [Bugsnag configuration].appVersion), @"app");
    BSGDictSetSafeObject(dict, BSGParseDeviceState(systemInfo), @"device");
    return [NSDictionary dictionaryWithDictionary:dict];
}

@end
