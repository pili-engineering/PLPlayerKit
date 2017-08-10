//
//  QNDes.h
//  HappyDNS
//
//  Created by bailong on 15/8/1.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import <Foundation/Foundation.h>

extern const int kQN_ENCRYPT_FAILED;
extern const int kQN_DECRYPT_FAILED;

@interface QNDes : NSObject

- (NSData *)encrypt:(NSData *)input;

- (NSData *)decrpyt:(NSData *)input;

- (instancetype)init:(NSData *)key;

@end
