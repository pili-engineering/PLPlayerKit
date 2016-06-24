//
//  QNResolverDelegate.h
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import <Foundation/Foundation.h>

extern const int kQNDomainHijackingCode;
extern const int kQNDomainNotOwnCode;
extern const int kQNDomainSeverError;

@class QNDomain;
@class QNNetworkInfo;
@protocol QNResolverDelegate <NSObject>
- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError **)error;
@end
