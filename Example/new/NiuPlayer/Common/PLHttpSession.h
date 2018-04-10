//
//  PLHttpSession.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/7.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PLHttpSession : NSObject

+ (void)requestShortMediaList:(void (^)(NSArray *list, NSError *error))completeBlock;

+ (void)requestLongMediaList:(void (^)(NSArray *list, NSError *error))completeBlock;

+ (void)requestLiveMediaList:(void (^)(NSArray *list, NSError *error))completeBlock;

@end
