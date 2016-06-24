//
//  QNHijackingDetectWrapper.h
//  HappyDNS
//
//  Created by bailong on 15/7/16.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "QNResolverDelegate.h"

@class QNResolver;
@interface QNHijackingDetectWrapper : NSObject <QNResolverDelegate>
- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError *__autoreleasing *)error;
- (instancetype)initWithResolver:(QNResolver *)resolver;
@end
