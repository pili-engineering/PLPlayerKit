//
//  NSError+Pili.h
//  PLPlayerKit
//
//  Created by 0dayZh on 15/10/17.
//  Copyright © 2015年 qgenius. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PLPlayerError.h"

extern NSString *PLPlayerErrorDomain;

@interface NSError (Pili)

+ (NSError *)errorWithPlayerError:(PLPlayerError)playerError userInfo:(NSDictionary *)userInfo;

+ (NSError *)errorWithPlayerError:(PLPlayerError)playerError message:(const char *)message;

@end
