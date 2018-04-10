//
//  QNHex.h
//  HappyDNS
//
//  Created by bailong on 15/7/31.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import <Foundation/Foundation.h>

char *qn_encodeHexData(char *output_buf, const char *data, int data_size, BOOL up);

@interface QNHex : NSObject

+ (NSString *)encodeHexData:(NSData *)data;
+ (NSString *)encodeHexString:(NSString *)str;

+ (NSData *)decodeHexString:(NSString *)hex;
+ (NSString *)decodeHexToString:(NSString *)hex;

@end
