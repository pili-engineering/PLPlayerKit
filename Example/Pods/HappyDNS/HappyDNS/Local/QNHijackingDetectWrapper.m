//
//  QNHijackingDetectWrapper.m
//  HappyDNS
//
//  Created by bailong on 15/7/16.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNHijackingDetectWrapper.h"
#import "QNDomain.h"
#import "QNRecord.h"
#import "QNResolver.h"

@interface QNHijackingDetectWrapper ()
@property (nonatomic, readonly) QNResolver *resolver;
@end

@implementation QNHijackingDetectWrapper

- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError *__autoreleasing *)error {
    NSArray *result = [_resolver query:domain networkInfo:netInfo error:error];
    if (((!domain.hasCname) && domain.maxTtl == 0) || result == nil || result.count == 0) {
        return result;
    }
    BOOL hasCname = NO;
    BOOL outOfTtl = NO;
    for (int i = 0; i < result.count; i++) {
        QNRecord *record = [result objectAtIndex:i];
        if (record.type == kQNTypeCname) {
            hasCname = YES;
        }
        if (domain.maxTtl > 0 && record.type == kQNTypeA && record.ttl > domain.maxTtl) {
            outOfTtl = YES;
        }
    }
    if ((domain.hasCname && !hasCname) || outOfTtl) {
        if (error != nil) {
            *error = [[NSError alloc] initWithDomain:domain.domain code:kQNDomainHijackingCode userInfo:nil];
        }
        return nil;
    }
    return result;
}
- (instancetype)initWithResolver:(QNResolver *)resolver {
    if (self = [super init]) {
        _resolver = resolver;
    }
    return self;
}

@end
