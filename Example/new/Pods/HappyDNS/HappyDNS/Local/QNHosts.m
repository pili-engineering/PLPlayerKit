//
//  QNHosts.m
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNHosts.h"
#import "QNDomain.h"
//#import "QNIP.h"
#import "QNNetworkInfo.h"

@interface QNHosts ()
@property (nonatomic) NSMutableDictionary *dict;
@end

@interface QNHostsValue : NSObject
@property (nonatomic, copy, readonly) NSString *ip;
@property (readonly) int provider;
@end

@implementation QNHostsValue

- (instancetype)init:(NSString *)ip provider:(int)provider {
    if (self = [super init]) {
        _ip = ip;
        _provider = provider;
    }
    return self;
}

@end

static NSArray *filter(NSArray *input, int provider) {
    NSMutableArray *normal = [[NSMutableArray alloc] initWithCapacity:input.count];
    NSMutableArray *special = [[NSMutableArray alloc] init];
    for (QNHostsValue *v in input) {
        NSString *ip = v.ip;
        if (v.provider == kQNISP_GENERAL) {
            [normal addObject:ip];
        }
        if (provider == v.provider && provider != kQNISP_GENERAL) {
            [special addObject:ip];
        }
    }
    if (special.count != 0) {
        return special;
    }
    return normal;
}

@implementation QNHosts
- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo {
    NSMutableArray *x;
    @synchronized(_dict) {
        x = [_dict objectForKey:domain.domain];
    }

    if (x == nil || x.count == 0) {
        return nil;
    }
    if (x.count >= 2) {
        QNHostsValue *first = [x firstObject];
        [x removeObjectAtIndex:0];
        [x addObject:first];
    }
    return filter(x, netInfo.provider);
}

- (void)put:(NSString *)domain ip:(NSString *)ip {
    [self put:domain ip:ip provider:kQNISP_GENERAL];
}

- (void)put:(NSString *)domain ip:(NSString *)ip provider:(int)provider {
    QNHostsValue *v = [[QNHostsValue alloc] init:ip provider:provider];
    @synchronized(_dict) {
        NSMutableArray *x = [_dict objectForKey:domain];
        if (x == nil) {
            x = [[NSMutableArray alloc] init];
        }
        [x addObject:v];
        [_dict setObject:x forKey:domain];
    }
}

- (instancetype)init {
    if (self = [super init]) {
        _dict = [[NSMutableDictionary alloc] init];
    }
    return self;
}

@end
