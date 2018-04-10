//
//  QNLruCache.h
//  HappyDNS
//
//  Created by bailong on 16/7/5.
//  Copyright © 2016年 Qiniu Cloud Storage. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface QNLruCache : NSObject

- (instancetype)init:(NSUInteger)limit;

- (void)removeAllObjects;

- (void)removeObjectForKey:(NSString *)key;

- (id)objectForKey:(NSString *)key;

- (void)setObject:(id)obj forKey:(NSString *)key;

@end
