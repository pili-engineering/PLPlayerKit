//
//  PLPlayerEnv.h
//  PLPlayerKit
//
//  Created by 孙菲 on 16/7/11.
//  Copyright © 2016年 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PLPlayerEnv : NSObject

/**
 @brief 初始化 player 的运行环境，需要在 -application:didFinishLaunchingWithOptions: 方法下调用该方法，
 
 */
+(void)initEnv;

/**
 @brief 判断当前环境是否已经初始化
 
 @return 已初始化返回 YES，否则为 NO
 */
+(BOOL)isInited;

/**
 @brief 是否开启 QoS，默认开启
 
 @param flag 开启为 YES，否则为NO
 */
+(void)enableQos:(BOOL)flag;

/**
 @brief 设置 ffmpeg 的版本号
 
 @param version const char *
 */
+ (void)setFFmpegVersion:(const char *)version;

+ (NSString *)getFFmpegVersion;

@end
