//
//  BugsnagKSCrashSysInfoParser.h
//  Bugsnag
//
//  Created by Jamie Lynch on 28/11/2017.
//  Copyright © 2017 Bugsnag. All rights reserved.
//

#import <Foundation/Foundation.h>

NSDictionary *_Nonnull BSGParseDevice(NSDictionary *_Nonnull report);
NSDictionary *_Nonnull BSGParseApp(NSDictionary *_Nonnull report);
NSDictionary *_Nonnull BSGParseAppState(NSDictionary *_Nonnull report, NSString *_Nullable preferredVersion);
NSDictionary *_Nonnull BSGParseDeviceState(NSDictionary *_Nonnull report);
