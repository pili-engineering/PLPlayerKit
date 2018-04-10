//
//  QNResolver.m
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#include <arpa/inet.h>
#include <resolv.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#import "QNDomain.h"
#import "QNIP.h"
#import "QNRecord.h"
#import "QNResolver.h"

#import "QNResolvUtil.h"

#if __IPHONE_OS_VERSION_MIN_REQUIRED
#import <MobileCoreServices/MobileCoreServices.h>
#import <UIKit/UIKit.h>
#endif

@interface QNResolver ()
@property (nonatomic, readonly, strong) NSString *address;
@property (nonatomic, readonly) NSUInteger timeout;
@end

static NSArray *query_ip_v4(res_state res, const char *host) {
    u_char answer[2000];
    int len = res_nquery(res, host, ns_c_in, ns_t_a, answer, sizeof(answer));

    ns_msg handle;
    ns_initparse(answer, len, &handle);

    int count = ns_msg_count(handle, ns_s_an);
    if (count <= 0) {
        res_ndestroy(res);
        return nil;
    }
    NSMutableArray *array = [[NSMutableArray alloc] initWithCapacity:count];
    char buf[32];
    char cnameBuf[NS_MAXDNAME];
    memset(cnameBuf, 0, sizeof(cnameBuf));
    for (int i = 0; i < count; i++) {
        ns_rr rr;
        if (ns_parserr(&handle, ns_s_an, i, &rr) != 0) {
            res_ndestroy(res);
            return nil;
        }
        int t = ns_rr_type(rr);
        int ttl = ns_rr_ttl(rr);
        NSString *val;
        if (t == ns_t_a) {
            const char *p = inet_ntop(AF_INET, ns_rr_rdata(rr), buf, sizeof(buf));
            val = [NSString stringWithUTF8String:p];
        } else if (t == ns_t_cname) {
            int x = ns_name_uncompress(answer, &(answer[len]), ns_rr_rdata(rr), cnameBuf, sizeof(cnameBuf));
            if (x <= 0) {
                continue;
            }
            val = [NSString stringWithUTF8String:cnameBuf];
            memset(cnameBuf, 0, sizeof(cnameBuf));
        } else {
            continue;
        }
        QNRecord *record = [[QNRecord alloc] init:val ttl:ttl type:t];
        [array addObject:record];
    }
    res_ndestroy(res);
    return array;
}

@implementation QNResolver
- (instancetype)initWithAddres:(NSString *)address {
    return [self initWithAddress:address];
}

- (instancetype)initWithAddress:(NSString *)address {
    return [self initWithAddress:address timeout:QN_DNS_DEFAULT_TIMEOUT];
}

- (instancetype)initWithAddress:(NSString *)address
                        timeout:(NSUInteger)time {
    if (self = [super init]) {
        _address = address;
        _timeout = time;
    }
    return self;
}

- (NSArray *)query:(QNDomain *)domain networkInfo:(QNNetworkInfo *)netInfo error:(NSError *__autoreleasing *)error {
    struct __res_state res;

    int r = setup_dns_server(&res, _address, _timeout);
    if (r != 0) {
        if (error != nil) {
            *error = [[NSError alloc] initWithDomain:@"qiniu.dns" code:kQNDomainSeverError userInfo:nil];
        }
        return nil;
    }

    NSArray *ret = query_ip_v4(&res, [domain.domain cStringUsingEncoding:NSUTF8StringEncoding]);
    if (ret != nil && ret.count != 0) {
        return ret;
    }
    if (error != nil) {
        *error = [[NSError alloc] initWithDomain:@"qiniu.dns" code:NSURLErrorDNSLookupFailed userInfo:nil];
    }
    return nil;
}

+ (instancetype)systemResolver {
    return [[QNResolver alloc] initWithAddress:nil];
}

+ (NSString *)systemDnsServer {
    struct __res_state res;
    int r = res_ninit(&res);
    if (r != 0) {
        return nil;
    }

    union res_sockaddr_union server[MAXNS] = {0};
    r = res_getservers(&res, server, MAXNS);
    res_ndestroy(&res);
    if (r <= 0) {
        return nil;
    }

    int family = server[0].sin.sin_family;
    char buf[64] = {0};
    const void *addr;
    if (family == AF_INET6) {
        addr = &server[0].sin6.sin6_addr;
    } else {
        addr = &server[0].sin.sin_addr;
    }
    const char *p = inet_ntop(family, addr, buf, sizeof(buf));
    if (p == NULL) {
        return nil;
    }
    return [NSString stringWithUTF8String:p];
}

@end
