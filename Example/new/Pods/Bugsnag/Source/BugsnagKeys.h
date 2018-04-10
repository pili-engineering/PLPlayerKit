//
//  BugsnagKeys.h
//  Bugsnag
//
//  Created by Jamie Lynch on 24/10/2017.
//  Copyright © 2017 Bugsnag. All rights reserved.
//

#ifndef BugsnagKeys_h
#define BugsnagKeys_h

#import <Foundation/Foundation.h>

static NSString *const BSGDefaultNotifyUrl = @"https://notify.bugsnag.com/";

static NSString *const BSGKeyException = @"exception";
static NSString *const BSGKeyMessage = @"message";
static NSString *const BSGKeyName = @"name";
static NSString *const BSGKeyTimestamp = @"timestamp";
static NSString *const BSGKeyType = @"type";
static NSString *const BSGKeyMetaData = @"metaData";
static NSString *const BSGKeyId = @"id";
static NSString *const BSGKeyUser = @"user";
static NSString *const BSGKeyEmail = @"email";
static NSString *const BSGKeyDevelopment = @"development";
static NSString *const BSGKeyProduction = @"production";
static NSString *const BSGKeyReleaseStage = @"releaseStage";
static NSString *const BSGKeyConfig = @"config";
static NSString *const BSGKeyContext = @"context";
static NSString *const BSGKeyAppVersion = @"appVersion";
static NSString *const BSGKeyNotifyReleaseStages = @"notifyReleaseStages";
static NSString *const BSGKeyApiKey = @"apiKey";
static NSString *const BSGKeyNotifier = @"notifier";
static NSString *const BSGKeyEvents = @"events";
static NSString *const BSGKeyVersion = @"version";
static NSString *const BSGKeySeverity = @"severity";
static NSString *const BSGKeyUrl = @"url";
static NSString *const BSGKeyBatteryLevel = @"batteryLevel";
static NSString *const BSGKeyDeviceState = @"deviceState";
static NSString *const BSGKeyCharging = @"charging";
static NSString *const BSGKeyLabel = @"label";
static NSString *const BSGKeySeverityReason = @"severityReason";
static NSString *const BSGKeyLogLevel = @"logLevel";
static NSString *const BSGKeyOrientation = @"orientation";
static NSString *const BSGKeySimulatorModelId = @"SIMULATOR_MODEL_IDENTIFIER";
static NSString *const BSGKeyFrameAddrFormat = @"0x%lx";
static NSString *const BSGKeySymbolAddr = @"symbolAddress";
static NSString *const BSGKeyMachoLoadAddr = @"machoLoadAddress";
static NSString *const BSGKeyIsPC = @"isPC";
static NSString *const BSGKeyIsLR = @"isLR";
static NSString *const BSGKeyMachoFile = @"machoFile";
static NSString *const BSGKeyMachoUUID = @"machoUUID";
static NSString *const BSGKeyMachoVMAddress = @"machoVMAddress";
static NSString *const BSGKeyCppException = @"cpp_exception";
static NSString *const BSGKeyExceptionName = @"exception_name";
static NSString *const BSGKeyMach = @"mach";
static NSString *const BSGKeySignal = @"signal";
static NSString *const BSGKeyReason = @"reason";
static NSString *const BSGKeyInfo = @"info";
static NSString *const BSGKeyWarning = @"warning";
static NSString *const BSGKeyError = @"error";
static NSString *const BSGKeyOsVersion = @"osVersion";
static NSString *const BSGKeySystem = @"system";
static NSString *const BSGKeyStacktrace = @"stacktrace";
static NSString *const BSGKeyGroupingHash = @"groupingHash";
static NSString *const BSGKeyErrorClass = @"errorClass";
static NSString *const BSGKeyBreadcrumbs = @"breadcrumbs";
static NSString *const BSGKeyThreads = @"threads";
static NSString *const BSGKeyExceptions = @"exceptions";
static NSString *const BSGKeyPayloadVersion = @"payloadVersion";
static NSString *const BSGKeyDevice = @"device";
static NSString *const BSGKeyAppState = @"app";
static NSString *const BSGKeyApp = @"app";
static NSString *const BSGKeyUnhandled = @"unhandled";
static NSString *const BSGKeyAttributes = @"attributes";
static NSString *const BSGKeyAction = @"action";
static NSString *const BSGKeySession = @"session";


static NSString *const BSGKeyExecutableName = @"CFBundleExecutable";
static NSString *const BSGKeyHwModel = @"hw.model";
static NSString *const BSGKeyHwMachine = @"hw.machine";

#define BSGKeyHwCputype "hw.cputype"
#define BSGKeyHwCpusubtype "hw.cpusubtype"
#define BSGKeyDefaultMacName "en0"

#endif /* BugsnagKeys_h */
