//
//  QNLruCache.m
//  HappyDNS
//
//  Created by bailong on 16/7/5.
//  Copyright © 2016年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNLruCache.h"

@interface QNLruCache ()

@property (nonatomic, readonly) NSUInteger limit;

@property (nonatomic, readonly) NSMutableDictionary* cache;

@property (nonatomic, readonly) NSMutableArray* list;

@end

@interface _QNElement : NSObject
@property (nonatomic, readonly, strong) NSString* key;
@property (nonatomic, strong) id obj;
- (instancetype)initObject:(id)obj forKey:(NSString*)key;
@end

@implementation _QNElement

- (instancetype)initObject:(id)obj forKey:(NSString*)key {
    if (self = [super init]) {
        _key = key;
        _obj = obj;
    }
    return self;
}

@end

@implementation QNLruCache

- (instancetype)init:(NSUInteger)limit {
    if (self = [super init]) {
        _limit = limit;
        _cache = [NSMutableDictionary new];
        _list = [NSMutableArray new];
    }
    return self;
}

- (void)removeAllObjects {
    [_cache removeAllObjects];
    [_list removeAllObjects];
}

- (void)removeObjectForKey:(NSString*)key {
    _QNElement* obj = [_cache objectForKey:key];
    if (obj == nil) {
        return;
    }
    [_cache removeObjectForKey:key];
    [_list removeObjectIdenticalTo:obj];
}

- (id)objectForKey:(NSString*)key {
    _QNElement* obj = [_cache objectForKey:key];
    if (obj != nil) {
        [_list removeObjectIdenticalTo:obj];
        [_list insertObject:obj atIndex:0];
    }
    return obj.obj;
}

- (void)setObject:(id)obj forKey:(NSString*)key {
    _QNElement* old = [_cache objectForKey:key];
    if (old) {
        old.obj = obj;
        [_list removeObjectIdenticalTo:old];
        [_list insertObject:old atIndex:0];
        return;
    } else if (_list.count == _limit) {
        old = [_list lastObject];
        [_list removeLastObject];
        [_cache removeObjectForKey:old.key];
    }
    _QNElement* newElement = [[_QNElement alloc] initObject:obj forKey:key];
    [_cache setObject:newElement forKey:key];
    [_list insertObject:newElement atIndex:0];
}

@end
