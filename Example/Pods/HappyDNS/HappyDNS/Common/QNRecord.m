//
//  QNRecord.m
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNRecord.h"

const int kQNTypeA = 1;
const int kQNTypeAAAA = 28;
const int kQNTypeCname = 5;
const int kQNTypeTXT = 16;

@implementation QNRecord
- (instancetype)init:(NSString *)value
                 ttl:(int)ttl
                type:(int)type {
    if (self = [super init]) {
        _value = value;
        _type = type;
        _ttl = ttl;
        _timeStamp = [[NSDate date] timeIntervalSince1970];
    }
    return self;
}

- (BOOL)expired:(long long)time {
    return time > _timeStamp + _ttl;
}

@end
