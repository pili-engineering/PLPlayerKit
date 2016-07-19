//
//  QNResolver.h
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNResolverDelegate.h"
#import <Foundation/Foundation.h>

@interface QNResolver : NSObject <QNResolverDelegate>
- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError *__autoreleasing *)error;

// @deprecated typo
- (instancetype)initWithAddres:(NSString *)address DEPRECATED_ATTRIBUTE;

- (instancetype)initWithAddress:(NSString *)address;

- (instancetype)initWithAddress:(NSString *)address
                        timeout:(NSUInteger)time;

+ (instancetype)systemResolver;
+ (NSString *)systemDnsServer;
@end
