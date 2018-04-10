//
//  QNDnspodFree.m
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNDnspodEnterprise.h"
#import <CommonCrypto/CommonCryptor.h>

#import "QNDes.h"
#import "QNDomain.h"
#import "QNHex.h"
#import "QNIP.h"
#import "QNRecord.h"

const int kQN_ENCRYPT_FAILED = -10001;
const int kQN_DECRYPT_FAILED = -10002;

@interface QNDnspodEnterprise ()
@property (readonly, strong) NSString *server;
@property (nonatomic, strong) NSString *userId;
@property (nonatomic, strong) QNDes *des;
@property (nonatomic) NSUInteger timeout;

@end

@implementation QNDnspodEnterprise

- (instancetype)initWithId:(NSString *)userId
                       key:(NSString *)key {
    return [self initWithId:userId key:key server:@"119.29.29.29"];
}

- (instancetype)initWithId:(NSString *)userId
                       key:(NSString *)key
                    server:(NSString *)server {
    return [self initWithId:userId key:key server:@"119.29.29.29" timeout:QN_DNS_DEFAULT_TIMEOUT];
}

- (instancetype)initWithId:(NSString *)userId
                       key:(NSString *)key
                    server:(NSString *)server
                   timeout:(NSUInteger)time {
    if (self = [super init]) {
        _server = server;
        _userId = userId;
        _des = [[QNDes alloc] init:[key dataUsingEncoding:NSUTF8StringEncoding]];
        _timeout = time;
    }
    return self;
}

- (NSString *)encrypt:(NSString *)domain {
    NSData *data = [_des encrypt:[domain dataUsingEncoding:NSUTF8StringEncoding]];
    if (data == nil) {
        return nil;
    }
    NSString *str = [QNHex encodeHexData:data];
    return str;
}

- (NSString *)decrypt:(NSData *)raw {
    NSData *enc = [QNHex decodeHexString:[[NSString alloc] initWithData:raw
                                                               encoding:NSUTF8StringEncoding]];
    if (enc == nil) {
        return nil;
    }
    NSData *data = [_des decrpyt:enc];
    if (data == nil) {
        return nil;
    }
    return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
}

- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError *__autoreleasing *)error {
    NSString *encrypt = [self encrypt:domain.domain];
    if (encrypt == nil) {
        if (error != nil) {
            *error = [[NSError alloc] initWithDomain:domain.domain code:kQN_ENCRYPT_FAILED userInfo:nil];
        }
        return nil;
    }
    NSString *url = [NSString stringWithFormat:@"http://%@/d?ttl=1&dn=%@&id=%@", [QNIP ipHost:_server], encrypt, _userId];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:[NSURL URLWithString:url] cachePolicy:NSURLRequestUseProtocolCachePolicy timeoutInterval:_timeout];
    NSHTTPURLResponse *response = nil;
    NSError *httpError = nil;
    NSData *data = [NSURLConnection sendSynchronousRequest:urlRequest
                                         returningResponse:&response
                                                     error:&httpError];

    if (httpError != nil) {
        if (error != nil) {
            *error = httpError;
        }
        return nil;
    }
    if (response.statusCode != 200) {
        return nil;
    }

    NSString *raw = [self decrypt:data];
    if (raw == nil) {
        if (error != nil) {
            *error = [[NSError alloc] initWithDomain:domain.domain code:kQN_DECRYPT_FAILED userInfo:nil];
        }
        return nil;
    }
    NSArray *ip1 = [raw componentsSeparatedByString:@","];
    if (ip1.count != 2) {
        return nil;
    }
    NSString *ttlStr = [ip1 objectAtIndex:1];
    int ttl = [ttlStr intValue];
    if (ttl <= 0) {
        return nil;
    }
    NSString *ips = [ip1 objectAtIndex:0];
    NSArray *ipArray = [ips componentsSeparatedByString:@";"];
    NSMutableArray *ret = [[NSMutableArray alloc] initWithCapacity:ipArray.count];
    for (int i = 0; i < ipArray.count; i++) {
        QNRecord *record = [[QNRecord alloc] init:[ipArray objectAtIndex:i] ttl:ttl type:kQNTypeA];
        [ret addObject:record];
    }
    return ret;
}

@end
