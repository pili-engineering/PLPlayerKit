//
//  QNDnsManager.m
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNDnsManager.h"
#import "QNDomain.h"
#import "QNHosts.h"
#import "QNIP.h"
#import "QNLruCache.h"
#import "QNNetworkInfo.h"
#import "QNRecord.h"
#import "QNResolverDelegate.h"

#include "QNGetAddrInfo.h"

const int kQNDomainHijackingCode = -7001;
const int kQNDomainNotOwnCode = -7002;
const int kQNDomainSeverError = -7003;

@interface QNDnsManager ()

@property (nonatomic, strong) QNLruCache *cache;
@property (atomic) QNNetworkInfo *curNetwork;
@property (nonatomic) NSArray *resolvers;
@property (atomic) UInt32 resolverStatus;
@property (nonatomic) QNHosts *hosts;
@property (nonatomic, strong) id<QNIpSorter> sorter;
@end

//static inline BOOL bits_isSet(UInt32 v, int index) {
//	return (v & (1 << index)) != 0;
//}

static inline UInt32 bits_set(UInt32 v, int bitIndex) {
    return v |= (1 << bitIndex);
}

static inline UInt32 bits_leadingZeros(UInt32 x) {
    UInt32 y;
    int n = 32;
    y = x >> 16;
    if (y != 0) {
        n = n - 16;
        x = y;
    }
    y = x >> 8;
    if (y != 0) {
        n = n - 8;
        x = y;
    }
    y = x >> 4;
    if (y != 0) {
        n = n - 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0) {
        n = n - 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0) {
        return n - 2;
    }
    return n - x;
}

static NSMutableArray *trimCname(NSArray *records) {
    NSMutableArray *array = [[NSMutableArray alloc] init];
    for (QNRecord *r in records) {
        if (r.type == kQNTypeA || r.type == kQNTypeAAAA) {
            [array addObject:r];
        }
    }
    return array;
}

static NSArray *records2Ips(NSArray *records) {
    NSMutableArray *array = [[NSMutableArray alloc] init];
    for (QNRecord *r in records) {
        [array addObject:r.value];
    }
    return array;
}

@interface DummySorter : NSObject <QNIpSorter>

@end

@implementation DummySorter

//sorted already
- (NSArray *)sort:(NSArray *)ips {
    return ips;
}

@end

@implementation QNDnsManager
- (NSArray *)query:(NSString *)domain {
    return [self queryWithDomain:[[QNDomain alloc] init:domain]];
}

- (NSArray *)queryWithDomain:(QNDomain *)domain {
    if (domain.domain == nil) {
        return nil;
    }
    if ([QNIP mayBeIpV4:domain.domain]) {
        return [NSArray arrayWithObject:domain.domain];
    }
    NSArray *ips = [self queryInternalWithDomain:domain];
    return [_sorter sort:ips];
}

- (NSArray *)queryInternalWithDomain:(QNDomain *)domain {
    if (domain.hostsFirst) {
        NSArray *ret = [_hosts query:domain networkInfo:_curNetwork];
        if (ret != nil && ret.count != 0) {
            return ret;
        }
    }
    NSMutableArray *result;

    if ([_curNetwork isEqualToInfo:[QNNetworkInfo normal]] && [QNNetworkInfo isNetworkChanged]) {
        @synchronized(_cache) {
            [_cache removeAllObjects];
        }
        _resolverStatus = 0;
    } else {
        @synchronized(_cache) {
            result = [_cache objectForKey:domain.domain];
            if (result != nil && result.count > 1) {
                QNRecord *first = [result firstObject];
                [result removeObjectAtIndex:0];
                [result addObject:first];
            }
        }
    }
    @synchronized(_cache) {
        if (result != nil && result.count > 0) {
            QNRecord *record = [result objectAtIndex:0];
            if (![record expired:[[NSDate date] timeIntervalSince1970]]) {
                return records2Ips(result);
            }
        }
    }

    NSArray *records = nil;
    NSError *error = nil;
    int firstOk = 32 - bits_leadingZeros(_resolverStatus);
    for (int i = 0; i < _resolvers.count; i++) {
        int pos = (firstOk + i) % _resolvers.count;
        id<QNResolverDelegate> resolver = [_resolvers objectAtIndex:pos];
        QNNetworkInfo *previousNetwork = _curNetwork;
        NSString *previousIp = [QNNetworkInfo getIp];
        records = [resolver query:domain networkInfo:previousNetwork error:&error];
        if (error != nil) {
            NSError *tmp = error;
            error = nil;
            if (tmp.code == kQNDomainNotOwnCode) {
                continue;
            }
        }

        if (records == nil || records.count == 0) {
            if (_curNetwork == previousNetwork && [previousIp isEqualToString:[QNNetworkInfo getIp]]) {
                _resolverStatus = bits_set(_resolverStatus, pos);
            }
        } else {
            NSMutableArray *ret = trimCname(records);
            if (_curNetwork == previousNetwork && [previousIp isEqualToString:[QNNetworkInfo getIp]]) {
                @synchronized(_cache) {
                    [_cache setObject:ret forKey:domain.domain];
                }
            }
            return records2Ips(ret);
        }
    }

    if (!domain.hostsFirst) {
        return [_hosts query:domain networkInfo:_curNetwork];
    }
    return nil;
}

- (instancetype)init:(NSArray *)resolvers networkInfo:(QNNetworkInfo *)netInfo {
    return [self init:resolvers networkInfo:netInfo sorter:nil];
}

- (instancetype)init:(NSArray *)resolvers networkInfo:(QNNetworkInfo *)netInfo sorter:(id<QNIpSorter>)sorter {
    if (self = [super init]) {
        _cache = [[QNLruCache alloc] init:1024];
        _curNetwork = netInfo;
        _resolvers = [[NSArray alloc] initWithArray:resolvers];
        _hosts = [[QNHosts alloc] init];
        if (sorter == nil) {
            _sorter = [[DummySorter alloc] init];
        } else {
            _sorter = sorter;
        }
    }
    return self;
}

- (void)onNetworkChange:(QNNetworkInfo *)netInfo {
    @synchronized(_cache) {
        [_cache removeAllObjects];
    }
    _curNetwork = netInfo;
}

- (instancetype)putHosts:(NSString *)domain ip:(NSString *)ip {
    [_hosts put:domain ip:ip];
    return self;
}

- (instancetype)putHosts:(NSString *)domain ip:(NSString *)ip provider:(int)provider {
    [_hosts put:domain ip:ip provider:provider];
    return self;
}

- (NSURL *)queryAndReplaceWithIP:(NSURL *)url {
    NSURLComponents *urlComponents = [[NSURLComponents alloc] initWithURL:url resolvingAgainstBaseURL:YES];
    if (!urlComponents) {
        return nil;
    }

    NSString *host = urlComponents.host;
    NSArray *ips = [self query:host];

    NSURL *URL = nil;
    if (ips && ips.count > 0) {
        urlComponents.host = [QNIP ipHost:ips[0]];
    }

    URL = urlComponents.URL;
    return URL;
}

static QNGetAddrInfoCallback getAddrInfoCallback = nil;
static qn_ips_ret *dns_callback_internal(const char *host) {
    if (getAddrInfoCallback == nil) {
        return NULL;
    }
    NSString *s = [[NSString alloc] initWithUTF8String:host];
    if (s == nil) {
        return NULL;
    }
    NSArray *ips = getAddrInfoCallback(s);
    if (ips == nil) {
        return NULL;
    }
    qn_ips_ret *ret = calloc(sizeof(char *), ips.count + 1);
    for (int i = 0; i < ips.count; i++) {
        NSString *ip = ips[i];
        char *ip2 = strdup([ip cStringUsingEncoding:NSUTF8StringEncoding]);
        ret->ips[i] = ip2;
    }
    return ret;
}
static qn_ips_ret *dns_callback(const char *host) {
    qn_ips_ret *ret = dns_callback_internal(host);
    if (ret == NULL) {
        //only for compatible
        qn_ips_ret *ret = calloc(sizeof(char *), 2);
        ret->ips[0] = strdup(host);
    }
    return ret;
}

static QNIpStatusCallback ipStatusCallback = nil;
static void ip_status_callback(const char *ip, int code, int time_ms) {
    if (ipStatusCallback == nil) {
        return;
    }
    NSString *s = [[NSString alloc] initWithUTF8String:ip];
    if (s == nil) {
        return;
    }
    ipStatusCallback(s, code, time_ms);
}

+ (void)setGetAddrInfoBlock:(QNGetAddrInfoCallback)block {
    if ([QNIP isIpV6FullySupported] || ![QNIP isV6]) {
        getAddrInfoCallback = block;
        qn_set_dns_callback(dns_callback);
    }
}

+ (void)setDnsManagerForGetAddrInfo:(QNDnsManager *)dns {
    [QNDnsManager setGetAddrInfoBlock:^NSArray *(NSString *host) {
        return [dns query:host];
    }];
}

+ (void)setIpStatusCallback:(QNIpStatusCallback)block {
    ipStatusCallback = block;
    qn_set_ip_report_callback(ip_status_callback);
}

+ (BOOL)needHttpDns {
    NSTimeZone *timeZone = [NSTimeZone localTimeZone];
    NSString *tzName = [timeZone name];
    return [tzName isEqual:@"Asia/Shanghai"] || [tzName isEqual:@"Asia/Chongqing"] || [tzName isEqual:@"Asia/Harbin"] || [tzName isEqual:@"Asia/Urumqi"];
}
@end
