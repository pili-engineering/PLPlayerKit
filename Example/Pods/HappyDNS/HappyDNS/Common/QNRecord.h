//
//  QNRecord.h
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 *    A 记录
 */
extern const int kQNTypeA;

/**
 *    AAAA 记录
 */
extern const int kQNTypeAAAA;

/**
 *  Cname 记录
 */
extern const int kQNTypeCname;

/**
 *  Txt 记录
 */
extern const int kQNTypeTXT;

@interface QNRecord : NSObject
@property (nonatomic, strong, readonly) NSString *value;
@property (nonatomic, readonly) int ttl;
@property (nonatomic, readonly) int type;
@property (nonatomic, readonly) long long timeStamp;

- (instancetype)init:(NSString *)value
                 ttl:(int)ttl
                type:(int)type;

- (BOOL)expired:(long long)time;
@end
