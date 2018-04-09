//
//  QNDnspodFree.m
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNDnspodFree.h"
#import "QNDomain.h"
#import "QNIP.h"
#import "QNRecord.h"

@interface QNDnspodFree ()
@property (readonly, nonatomic, strong) NSString *server;
@property (readonly, nonatomic) NSUInteger timeout;
@end

@implementation QNDnspodFree

- (instancetype)init {
    return [self initWithServer:@"119.29.29.29"];
}
- (instancetype)initWithServer:(NSString *)server {
    return [self initWithServer:@"119.29.29.29" timeout:QN_DNS_DEFAULT_TIMEOUT];
}

- (instancetype)initWithServer:(NSString *)server
                       timeout:(NSUInteger)time {
    if (self = [super init]) {
        _server = server;
        _timeout = time;
    }
    return self;
}

- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError *__autoreleasing *)error {
    NSString *url = [NSString stringWithFormat:@"http://%@/d?ttl=1&dn=%@", [QNIP ipHost:_server], domain.domain];
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
    NSString *raw = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
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
