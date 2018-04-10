//
//  KSSystemInfo.m
//
//  Created by Karl Stenerud on 2012-02-05.
//
//  Copyright (c) 2012 Karl Stenerud. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#import "BSG_KSSystemInfo.h"
#import "BSG_KSSystemInfoC.h"
#import "BSG_KSDynamicLinker.h"
#import "BSG_KSJSONCodecObjC.h"
#import "BSG_KSMach.h"
#import "BSG_KSSafeCollections.h"
#import "BSG_KSSysCtl.h"
#import "BSG_KSSystemCapabilities.h"
#import "BugsnagKeys.h"
#import "BSG_KSLogger.h"

#import <CommonCrypto/CommonDigest.h>
#if BSG_KSCRASH_HAS_UIKIT
#import <UIKit/UIKit.h>
#endif

@implementation BSG_KSSystemInfo

// ============================================================================
#pragma mark - Utility -
// ============================================================================

/** Get a sysctl value as an NSNumber.
 *
 * @param name The sysctl name.
 *
 * @return The result of the sysctl call.
 */
+ (NSNumber *)int32Sysctl:(NSString *)name {
    return @(bsg_kssysctl_int32ForName(
            [name cStringUsingEncoding:NSUTF8StringEncoding]));
}

/** Get a sysctl value as an NSNumber.
 *
 * @param name The sysctl name.
 *
 * @return The result of the sysctl call.
 */
+ (NSNumber *)int64Sysctl:(NSString *)name {
    return @(bsg_kssysctl_int64ForName([name
            cStringUsingEncoding:NSUTF8StringEncoding]));
}

/** Get a sysctl value as an NSString.
 *
 * @param name The sysctl name.
 *
 * @return The result of the sysctl call.
 */
+ (NSString *)stringSysctl:(NSString *)name {
    NSString *str = nil;
    size_t size = bsg_kssysctl_stringForName(
        [name cStringUsingEncoding:NSUTF8StringEncoding], NULL, 0);

    if (size <= 0) {
        return @"";
    }

    NSMutableData *value = [NSMutableData dataWithLength:size];

    if (bsg_kssysctl_stringForName(
            [name cStringUsingEncoding:NSUTF8StringEncoding],
            value.mutableBytes, size) != 0) {
        str = [NSString stringWithCString:value.mutableBytes
                                 encoding:NSUTF8StringEncoding];
    }

    return str;
}

/** Get a sysctl value as an NSDate.
 *
 * @param name The sysctl name.
 *
 * @return The result of the sysctl call.
 */
+ (NSDate *)dateSysctl:(NSString *)name {
    NSDate *result = nil;

    struct timeval value = bsg_kssysctl_timevalForName(
        [name cStringUsingEncoding:NSUTF8StringEncoding]);
    if (!(value.tv_sec == 0 && value.tv_usec == 0)) {
        result =
            [NSDate dateWithTimeIntervalSince1970:(NSTimeInterval)value.tv_sec];
    }

    return result;
}

/** Convert raw UUID bytes to a human-readable string.
 *
 * @param uuidBytes The UUID bytes (must be 16 bytes long).
 *
 * @return The human readable form of the UUID.
 */
+ (NSString *)uuidBytesToString:(const uint8_t *)uuidBytes {
    CFUUIDRef uuidRef =
        CFUUIDCreateFromUUIDBytes(NULL, *((CFUUIDBytes *)uuidBytes));
    NSString *str =
        (__bridge_transfer NSString *)CFUUIDCreateString(NULL, uuidRef);
    CFRelease(uuidRef);

    return str;
}

/** Get this application's executable path.
 *
 * @return Executable path.
 */
+ (NSString *)executablePath {
    NSBundle *mainBundle = [NSBundle mainBundle];
    NSDictionary *infoDict = [mainBundle infoDictionary];
    NSString *bundlePath = [mainBundle bundlePath];
    NSString *executableName = infoDict[BSGKeyExecutableName];
    return [bundlePath stringByAppendingPathComponent:executableName];
}

/** Get this application's UUID.
 *
 * @return The UUID.
 */
+ (NSString *)appUUID {
    NSString *result = nil;

    NSString *exePath = [self executablePath];

    if (exePath != nil) {
        const uint8_t *uuidBytes =
            bsg_ksdlimageUUID([exePath UTF8String], true);
        if (uuidBytes == NULL) {
            // OSX app image path is a lie.
            uuidBytes = bsg_ksdlimageUUID(
                [exePath.lastPathComponent UTF8String], false);
        }
        if (uuidBytes != NULL) {
            result = [self uuidBytesToString:uuidBytes];
        }
    }

    return result;
}

/** Generate a 20 byte SHA1 hash that remains unique across a single device and
 * application. This is slightly different from the Apple crash report key,
 * which is unique to the device, regardless of the application.
 *
 * @return The stringified hex representation of the hash for this device + app.
 */
+ (NSString *)deviceAndAppHash {
    NSMutableData *data = nil;

#if BSG_KSCRASH_HAS_UIDEVICE
    if ([[UIDevice currentDevice]
            respondsToSelector:@selector(identifierForVendor)]) {
        data = [NSMutableData dataWithLength:16];
        [[UIDevice currentDevice].identifierForVendor
            getUUIDBytes:data.mutableBytes];
    } else
#endif
    {
        data = [NSMutableData dataWithLength:6];
        bsg_kssysctl_getMacAddress(BSGKeyDefaultMacName, [data mutableBytes]);
    }

    // Append some device-specific data.
    [data appendData:(NSData * _Nonnull)[[self stringSysctl:BSGKeyHwMachine]
                         dataUsingEncoding:NSUTF8StringEncoding]];
    [data appendData:(NSData * _Nonnull)[[self stringSysctl:BSGKeyHwModel]
                         dataUsingEncoding:NSUTF8StringEncoding]];
    [data appendData:(NSData * _Nonnull)[[self currentCPUArch]
                         dataUsingEncoding:NSUTF8StringEncoding]];

    // Append the bundle ID.
    NSData *bundleID = [[[NSBundle mainBundle] bundleIdentifier]
        dataUsingEncoding:NSUTF8StringEncoding];
    if (bundleID != nil) {
        [data appendData:bundleID];
    }

    // SHA the whole thing.
    uint8_t sha[CC_SHA1_DIGEST_LENGTH];
    CC_SHA1([data bytes], (CC_LONG)[data length], sha);

    NSMutableString *hash = [NSMutableString string];
    for (size_t i = 0; i < sizeof(sha); i++) {
        [hash appendFormat:@"%02x", sha[i]];
    }

    return hash;
}

/** Get the current CPU's architecture.
 *
 * @return The current CPU archutecture.
 */
+ (NSString *)CPUArchForCPUType:(cpu_type_t)cpuType
                        subType:(cpu_subtype_t)subType {
    switch (cpuType) {
    case CPU_TYPE_ARM: {
        switch (subType) {
        case CPU_SUBTYPE_ARM_V6:
            return @"armv6";
        case CPU_SUBTYPE_ARM_V7:
            return @"armv7";
        case CPU_SUBTYPE_ARM_V7F:
            return @"armv7f";
        case CPU_SUBTYPE_ARM_V7K:
            return @"armv7k";
#ifdef CPU_SUBTYPE_ARM_V7S
        case CPU_SUBTYPE_ARM_V7S:
            return @"armv7s";
#endif
        }
        break;
    }
    case CPU_TYPE_X86:
        return @"x86";
    case CPU_TYPE_X86_64:
        return @"x86_64";
    }

    return nil;
}

+ (NSString *)currentCPUArch {
    NSString *result =
        [self CPUArchForCPUType:bsg_kssysctl_int32ForName(BSGKeyHwCputype)
                        subType:bsg_kssysctl_int32ForName(BSGKeyHwCpusubtype)];

    return result ?: [NSString stringWithUTF8String:bsg_ksmachcurrentCPUArch()];
}

/** Check if the current device is jailbroken.
 *
 * @return YES if the device is jailbroken.
 */
+ (BOOL)isJailbroken {
    return bsg_ksdlimageNamed("MobileSubstrate", false) != UINT32_MAX;
}

/** Check if the current build is a debug build.
 *
 * @return YES if the app was built in debug mode.
 */
+ (BOOL)isDebugBuild {
#ifdef DEBUG
    return YES;
#else
    return NO;
#endif
}

/** Check if this code is built for the simulator.
 *
 * @return YES if this is a simulator build.
 */
+ (BOOL)isSimulatorBuild {
#if TARGET_OS_SIMULATOR
    return YES;
#else
    return NO;
#endif
}

/** The file path for the bundle’s App Store receipt.
 *
 * @return App Store receipt for iOS 7+, nil otherwise.
 */
+ (NSString *)receiptUrlPath {
    NSString *path = nil;
#if BSG_KSCRASH_HOST_IOS
    // For iOS 6 compatibility
    if ([[UIDevice currentDevice].systemVersion
            compare:@"7"
            options:NSNumericSearch] != NSOrderedAscending) {
#endif
        path = [NSBundle mainBundle].appStoreReceiptURL.path;
#if BSG_KSCRASH_HOST_IOS
    }
#endif
    return path;
}

/** Check if the current build is a "testing" build.
 * This is useful for checking if the app was released through Testflight.
 *
 * @return YES if this is a testing build.
 */
+ (BOOL)isTestBuild {
    return [[self receiptUrlPath].lastPathComponent
        isEqualToString:@"sandboxReceipt"];
}

/** Check if the app has an app store receipt.
 * Only apps released through the app store will have a receipt.
 *
 * @return YES if there is an app store receipt.
 */
+ (BOOL)hasAppStoreReceipt {
    NSString *receiptPath = [self receiptUrlPath];
    if (receiptPath == nil) {
        return NO;
    }
    BOOL isAppStoreReceipt =
        [receiptPath.lastPathComponent isEqualToString:@"receipt"];
    BOOL receiptExists =
        [[NSFileManager defaultManager] fileExistsAtPath:receiptPath];

    return isAppStoreReceipt && receiptExists;
}

+ (NSString *)buildType {
    if ([BSG_KSSystemInfo isSimulatorBuild]) {
        return @"simulator";
    }
    if ([BSG_KSSystemInfo isDebugBuild]) {
        return @"debug";
    }
    if ([BSG_KSSystemInfo isTestBuild]) {
        return @"test";
    }
    if ([BSG_KSSystemInfo hasAppStoreReceipt]) {
        return @"app store";
    }
    return @"unknown";
}

// ============================================================================
#pragma mark - API -
// ============================================================================

+ (NSDictionary *)systemInfo {
    NSMutableDictionary *sysInfo = [NSMutableDictionary dictionary];

    NSBundle *mainBundle = [NSBundle mainBundle];
    NSDictionary *infoDict = [mainBundle infoDictionary];
    const struct mach_header *header = _dyld_get_image_header(0);

#if BSG_KSCRASH_HAS_UIDEVICE
    [sysInfo bsg_ksc_safeSetObject:[UIDevice currentDevice].systemName
                            forKey:@BSG_KSSystemField_SystemName];
    [sysInfo bsg_ksc_safeSetObject:[UIDevice currentDevice].systemVersion
                            forKey:@BSG_KSSystemField_SystemVersion];
#else
    [sysInfo bsg_ksc_safeSetObject:@"Mac OS"
                            forKey:@BSG_KSSystemField_SystemName];
    NSOperatingSystemVersion version =
        [NSProcessInfo processInfo].operatingSystemVersion;
    NSString *systemVersion;
    if (version.patchVersion == 0) {
        systemVersion =
            [NSString stringWithFormat:@"%ld.%ld", version.majorVersion,
                                       version.minorVersion];
    } else {
        systemVersion = [NSString
            stringWithFormat:@"%ld.%ld.%ld", version.majorVersion,
                             version.minorVersion, version.patchVersion];
    }
    [sysInfo bsg_ksc_safeSetObject:systemVersion
                            forKey:@BSG_KSSystemField_SystemVersion];
#endif
    if ([self isSimulatorBuild]) {
        NSString *model = [NSProcessInfo processInfo]
                              .environment[BSGKeySimulatorModelId];
        [sysInfo bsg_ksc_safeSetObject:model forKey:@BSG_KSSystemField_Machine];
        [sysInfo bsg_ksc_safeSetObject:@"simulator"
                                forKey:@BSG_KSSystemField_Model];
    } else {
#if BSG_KSCRASH_HOST_OSX
        // MacOS has the machine in the model field, and no model
        [sysInfo bsg_ksc_safeSetObject:[self stringSysctl:BSGKeyHwModel]
                                forKey:@BSG_KSSystemField_Machine];
#else
        [sysInfo bsg_ksc_safeSetObject:[self stringSysctl:BSGKeyHwMachine]
                                forKey:@BSG_KSSystemField_Machine];
        [sysInfo bsg_ksc_safeSetObject:[self stringSysctl:BSGKeyHwModel]
                                forKey:@BSG_KSSystemField_Model];
#endif
    }
    [sysInfo bsg_ksc_safeSetObject:[self stringSysctl:@"kern.version"]
                            forKey:@BSG_KSSystemField_KernelVersion];
    [sysInfo bsg_ksc_safeSetObject:[self stringSysctl:@"kern.osversion"]
                            forKey:@BSG_KSSystemField_OSVersion];
    [sysInfo bsg_ksc_safeSetObject:@([self isJailbroken])
                            forKey:@BSG_KSSystemField_Jailbroken];
    [sysInfo bsg_ksc_safeSetObject:[self dateSysctl:@"kern.boottime"]
                            forKey:@BSG_KSSystemField_BootTime];
    [sysInfo bsg_ksc_safeSetObject:[NSDate date]
                            forKey:@BSG_KSSystemField_AppStartTime];
    [sysInfo bsg_ksc_safeSetObject:[self executablePath]
                            forKey:@BSG_KSSystemField_ExecutablePath];
    [sysInfo bsg_ksc_safeSetObject:infoDict[BSGKeyExecutableName]
                            forKey:@BSG_KSSystemField_Executable];
    [sysInfo bsg_ksc_safeSetObject:infoDict[@"CFBundleIdentifier"]
                            forKey:@BSG_KSSystemField_BundleID];
    [sysInfo bsg_ksc_safeSetObject:infoDict[@"CFBundleName"]
                            forKey:@BSG_KSSystemField_BundleName];
    [sysInfo bsg_ksc_safeSetObject:infoDict[@"CFBundleVersion"]
                            forKey:@BSG_KSSystemField_BundleVersion];
    [sysInfo
        bsg_ksc_safeSetObject:infoDict[@"CFBundleShortVersionString"]
                       forKey:@BSG_KSSystemField_BundleShortVersion];
    [sysInfo bsg_ksc_safeSetObject:[self appUUID]
                            forKey:@BSG_KSSystemField_AppUUID];
    [sysInfo bsg_ksc_safeSetObject:[self currentCPUArch]
                            forKey:@BSG_KSSystemField_CPUArch];
    [sysInfo bsg_ksc_safeSetObject:[self int32Sysctl:@BSGKeyHwCputype]
                            forKey:@BSG_KSSystemField_CPUType];
    [sysInfo bsg_ksc_safeSetObject:[self int32Sysctl:@BSGKeyHwCpusubtype]
                            forKey:@BSG_KSSystemField_CPUSubType];
    [sysInfo bsg_ksc_safeSetObject:@(header->cputype)
                            forKey:@BSG_KSSystemField_BinaryCPUType];
    [sysInfo bsg_ksc_safeSetObject:@(header->cpusubtype)
                            forKey:@BSG_KSSystemField_BinaryCPUSubType];
    [sysInfo bsg_ksc_safeSetObject:[[NSTimeZone localTimeZone] abbreviation]
                            forKey:@BSG_KSSystemField_TimeZone];
    [sysInfo bsg_ksc_safeSetObject:[NSProcessInfo processInfo].processName
                            forKey:@BSG_KSSystemField_ProcessName];
    [sysInfo bsg_ksc_safeSetObject:@([NSProcessInfo processInfo]
                    .processIdentifier)
                            forKey:@BSG_KSSystemField_ProcessID];
    [sysInfo bsg_ksc_safeSetObject:@(getppid())
                            forKey:@BSG_KSSystemField_ParentProcessID];
    [sysInfo bsg_ksc_safeSetObject:[self deviceAndAppHash]
                            forKey:@BSG_KSSystemField_DeviceAppHash];
    [sysInfo bsg_ksc_safeSetObject:[BSG_KSSystemInfo buildType]
                            forKey:@BSG_KSSystemField_BuildType];

    NSDictionary *memory =
            @{@BSG_KSSystemField_Size: [self int64Sysctl:@"hw.memsize"]};
    [sysInfo bsg_ksc_safeSetObject:memory forKey:@BSG_KSSystemField_Memory];

    return sysInfo;
}

@end

const char *bsg_kssysteminfo_toJSON(void) {
    NSError *error;
    NSDictionary *systemInfo = [NSMutableDictionary
        dictionaryWithDictionary:[BSG_KSSystemInfo systemInfo]];
    NSMutableData *jsonData =
        (NSMutableData *)[BSG_KSJSONCodec encode:systemInfo
                                         options:BSG_KSJSONEncodeOptionSorted
                                           error:&error];
    if (error != nil) {
        BSG_KSLOG_ERROR(@"Could not serialize system info: %@", error);
        return NULL;
    }
    if (![jsonData isKindOfClass:[NSMutableData class]]) {
        jsonData = [NSMutableData dataWithData:jsonData];
    }

    [jsonData appendBytes:"\0" length:1];
    return strdup([jsonData bytes]);
}

char *bsg_kssysteminfo_copyProcessName(void) {
    return strdup([[NSProcessInfo processInfo].processName UTF8String]);
}
