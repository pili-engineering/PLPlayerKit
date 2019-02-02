//
//  BugsnagKSCrashSysInfoParser.m
//  Bugsnag
//
//  Created by Jamie Lynch on 28/11/2017.
//  Copyright © 2017 Bugsnag. All rights reserved.
//

#import "BugsnagKSCrashSysInfoParser.h"
#import "Bugsnag.h"
#import "BugsnagCollections.h"
#import "BugsnagKeys.h"
#import "BugsnagConfiguration.h"
#import "BugsnagLogger.h"

#define PLATFORM_WORD_SIZE sizeof(void*)*8

NSDictionary *BSGParseDevice(NSDictionary *report) {
    NSMutableDictionary *device = [NSMutableDictionary new];
    NSDictionary *state = [report valueForKeyPath:@"user.state.deviceState"];
    [device addEntriesFromDictionary:state];

    [device addEntriesFromDictionary:BSGParseDeviceState(report[@"system"])];
    
    BSGDictSetSafeObject(device, [[NSLocale currentLocale] localeIdentifier],
                         @"locale");
    
    BSGDictSetSafeObject(device, [report valueForKeyPath:@"system.time_zone"], @"timezone");
    BSGDictSetSafeObject(device, [report valueForKeyPath:@"system.memory.usable"],
                         @"totalMemory");
    
    BSGDictSetSafeObject(device,
                         [report valueForKeyPath:@"system.memory.free"],
                         @"freeMemory");
    
    BSGDictSetSafeObject(device, [report valueForKeyPath:@"report.timestamp"], @"time");
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray *searchPaths = NSSearchPathForDirectoriesInDomains(
                                                               NSDocumentDirectory, NSUserDomainMask, true);
    NSString *path = [searchPaths lastObject];
    
    NSError *error;
    NSDictionary *fileSystemAttrs =
    [fileManager attributesOfFileSystemForPath:path error:&error];
    
    if (error) {
        bsg_log_warn(@"Failed to read free disk space: %@", error);
    }
    
    NSNumber *freeBytes = [fileSystemAttrs objectForKey:NSFileSystemFreeSize];
    BSGDictSetSafeObject(device, freeBytes, @"freeDisk");
    BSGDictSetSafeObject(device, report[@"system"][@"device_app_hash"], @"id");

#if TARGET_OS_SIMULATOR
    BSGDictSetSafeObject(device, @YES, @"simulator");
#elif TARGET_OS_IPHONE || TARGET_OS_TV
    BSGDictSetSafeObject(device, @NO, @"simulator");
#endif

    return device;
}

NSDictionary *BSGParseApp(NSDictionary *report) {
    NSDictionary *system = report[BSGKeySystem];

    NSMutableDictionary *appState = [NSMutableDictionary dictionary];
    
    NSDictionary *stats = system[@"application_stats"];
    
    NSInteger activeTimeSinceLaunch =
    [stats[@"active_time_since_launch"] doubleValue] * 1000.0;
    NSInteger backgroundTimeSinceLaunch =
    [stats[@"background_time_since_launch"] doubleValue] * 1000.0;
    
    BSGDictSetSafeObject(appState, @(activeTimeSinceLaunch),
                         @"durationInForeground");

    BSGDictSetSafeObject(appState, system[BSGKeyExecutableName], BSGKeyName);
    BSGDictSetSafeObject(appState,
                         @(activeTimeSinceLaunch + backgroundTimeSinceLaunch),
                         @"duration");
    BSGDictSetSafeObject(appState, stats[@"application_in_foreground"],
                         @"inForeground");
    BSGDictSetSafeObject(appState, system[@"CFBundleIdentifier"], BSGKeyId);
    return appState;
}

NSDictionary *BSGParseAppState(NSDictionary *report, NSString *preferredVersion) {
    NSMutableDictionary *app = [NSMutableDictionary dictionary];

    NSString *version = preferredVersion ?: report[@"CFBundleShortVersionString"];

    BSGDictSetSafeObject(app, report[@"CFBundleVersion"], @"bundleVersion");
    BSGDictSetSafeObject(app, [Bugsnag configuration].releaseStage,
                         BSGKeyReleaseStage);
    BSGDictSetSafeObject(app, version, BSGKeyVersion);
    
    BSGDictSetSafeObject(app, [Bugsnag configuration].codeBundleId, @"codeBundleId");
    
    NSString *notifierType;
#if TARGET_OS_TV
    notifierType = @"tvOS";
#elif TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
    notifierType = @"iOS";
#elif TARGET_OS_MAC
    notifierType = @"macOS";
#endif
    
    if ([Bugsnag configuration].notifierType) {
        notifierType = [Bugsnag configuration].notifierType;
    }
    BSGDictSetSafeObject(app, notifierType, @"type");
    return app;
}

NSDictionary *BSGParseDeviceState(NSDictionary *report) {
    NSMutableDictionary *deviceState = [NSMutableDictionary new];
    BSGDictSetSafeObject(deviceState, report[@"model"], @"modelNumber");
    BSGDictSetSafeObject(deviceState, report[@"machine"], @"model");
    BSGDictSetSafeObject(deviceState, report[@"system_name"], @"osName");
    BSGDictSetSafeObject(deviceState, report[@"system_version"], @"osVersion");
    BSGDictSetSafeObject(deviceState, report[@"os_version"], @"osBuild");
    BSGDictSetSafeObject(deviceState, @(PLATFORM_WORD_SIZE), @"wordSize");
    BSGDictSetSafeObject(deviceState, @"Apple", @"manufacturer");
    BSGDictSetSafeObject(deviceState, report[@"jailbroken"], @"jailbroken");
    return deviceState;
}

