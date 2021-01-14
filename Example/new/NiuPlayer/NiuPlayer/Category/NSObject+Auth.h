//
//  NSObject+Auth.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/19.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSObject (Auth)

+ (void)haveAlbumAccess:(void(^)(BOOL isAuth))completeBlock;

+ (void)haveCameraAccess:(void(^)(BOOL isAuth))completeBlock;

@end
