//
//  QNDnspodFree.h
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNResolverDelegate.h"
#import <Foundation/Foundation.h>

@interface QNDnspodFree : NSObject <QNResolverDelegate>
@property (readonly, strong) NSString *server;
- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError *__autoreleasing *)error;

- (instancetype)init;
- (instancetype)initWithServer:(NSString *)server;

@end
