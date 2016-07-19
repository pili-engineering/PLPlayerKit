//
//  QNTxtResolver.h
//  HappyDNS
//
//  Created by bailong on 16/1/5.
//  Copyright © 2016年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNResolverDelegate.h"
#import <Foundation/Foundation.h>

@interface QNTxtResolver : NSObject <QNResolverDelegate>
- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError *__autoreleasing *)error;

/**
 *    根据服务器地址进行初始化
 *
 *    @param address DNS 服务器地址，nil 表示系统的
 */
- (instancetype)initWithAddress:(NSString *)address;

- (instancetype)initWithAddress:(NSString *)address timeout:(NSUInteger)time;
@end
