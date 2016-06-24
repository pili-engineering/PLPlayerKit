//
//  QNDomain.h
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface QNDomain : NSObject
@property (nonatomic, strong, readonly) NSString *domain;

// 用来判断劫持
@property (nonatomic, readonly) BOOL hasCname;
// 用来判断劫持
@property (nonatomic, readonly) int maxTtl;

@property (nonatomic, readonly) BOOL hostsFirst;

- (instancetype)init:(NSString *)domain;

- (instancetype)init:(NSString *)domain hostsFirst:(BOOL)hostsFirst hasCname:(BOOL)hasCname;

- (instancetype)init:(NSString *)domain hostsFirst:(BOOL)hostsFirst hasCname:(BOOL)hasCname maxTtl:(int)maxTtl;

@end
