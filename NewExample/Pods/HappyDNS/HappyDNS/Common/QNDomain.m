//
//  QNDomain.m
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNDomain.h"

@implementation QNDomain
- (instancetype)init:(NSString *)domain {
    return [self init:domain hostsFirst:NO hasCname:NO maxTtl:0];
}

- (instancetype)init:(NSString *)domain hostsFirst:(BOOL)hostsFirst hasCname:(BOOL)hasCname {
    return [self init:domain hostsFirst:hostsFirst hasCname:hasCname maxTtl:0];
}

- (instancetype)init:(NSString *)domain hostsFirst:(BOOL)hostsFirst hasCname:(BOOL)hasCname maxTtl:(int)maxTtl {
    if (self = [super init]) {
        _domain = domain;
        _hasCname = hasCname;
        _maxTtl = maxTtl;
        _hostsFirst = hostsFirst;
    }
    return self;
}

@end
