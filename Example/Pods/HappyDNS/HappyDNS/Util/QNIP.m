//
//  QNIPV6.m
//  HappyDNS
//
//  Created by bailong on 16/5/25.
//  Copyright © 2016年 Qiniu Cloud Storage. All rights reserved.
//

#import <arpa/inet.h>
#import <netdb.h>
#import <netinet/in.h>
#import <unistd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#import "QNHex.h"
#import "QNIP.h"

#if __IPHONE_OS_VERSION_MIN_REQUIRED
#import <MobileCoreServices/MobileCoreServices.h>
#import <UIKit/UIKit.h>
#endif

void qn_nat64(char *buf, int buf_size, uint32_t ipv4addr) {
    bzero(buf, buf_size);
    //nat 4 to ipv6
    const char *p = (const char *)&ipv4addr;
    const char prefix[] = "64:ff9b::";
    memcpy(buf, prefix, sizeof(prefix));
    char *phex = buf + sizeof(prefix) - 1;
    qn_encodeHexData(phex, p, 2, false);
    if (*phex == '0') {
        memmove(phex, phex + 1, 3);
        phex += 3;
    } else {
        phex += 4;
    }
    *phex = ':';
    phex++;
    qn_encodeHexData(phex, p + 2, 2, false);
    if (*phex == '0') {
        memmove(phex, phex + 1, 3);
        phex[3] = 0;
    }
}

int qn_local_ip_internal(char *buf, int buf_size, const char *t_ip) {
    struct addrinfo hints = {0}, *ai;
    int err = 0;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    int ret = getaddrinfo(t_ip, "53", &hints, &ai);
    if (ret != 0) {
        err = errno;
        return err;
    }

    int family = ai->ai_family;
    int sock = socket(family, ai->ai_socktype, 0);
    if (sock < 0) {
        err = errno;
        freeaddrinfo(ai);
        return err;
    }

    //fix getaddrinfo bug in ipv4 to ipv6
    if (ai->ai_family == AF_INET6) {
        ((struct sockaddr_in6 *)ai->ai_addr)->sin6_port = htons(53);
    }

    err = connect(sock, ai->ai_addr, ai->ai_addrlen);
    freeaddrinfo(ai);
    if (err < 0) {
        close(sock);
        err = errno;
        return err;
    }

    uint32_t localAddress[16] = {0};

    socklen_t addressLength = sizeof(localAddress);
    err = getsockname(sock, (struct sockaddr *)&localAddress, &addressLength);
    close(sock);
    if (err != 0) {
        return err;
    }
    void *addr;
    if (family == AF_INET6) {
        addr = &((struct sockaddr_in6 *)&localAddress)->sin6_addr;
    } else {
        addr = &((struct sockaddr_in *)&localAddress)->sin_addr;
    }
    const char *ip = inet_ntop(family, addr, buf, buf_size);
    if (ip == nil) {
        return -1;
    }
    return 0;
}

int qn_localIp(char *buf, int buf_size) {
    int ret = qn_local_ip_internal(buf, buf_size, "8.8.8.8");
    if (ret != 0) {
#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
        if (![QNIP isIpV6FullySupported]) {
            ret = qn_local_ip_internal(buf, buf_size, "64:ff9b::808:808");
        }
#endif
    }
    return ret;
}

@implementation QNIP

+ (BOOL)isV6 {
    struct addrinfo hints = {0}, *ai;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo("8.8.8.8", "http", &hints, &ai);
    if (ret != 0) {
        return NO;
    }
    int family = ai->ai_family;
    freeaddrinfo(ai);
    BOOL result = family == AF_INET6;
#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
    if (![QNIP isIpV6FullySupported] && !ret) {
        char buf[64] = {0};
        ret = qn_local_ip_internal(buf, sizeof(buf), "64:ff9b::808:808");
        if (strchr(buf, ':') != NULL) {
            result = YES;
        }
    }
#endif
    return result;
}

+ (NSString *)adaptiveIp:(NSString *)ipv4 {
    struct addrinfo hints = {0}, *ai;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo(ipv4.UTF8String, "http", &hints, &ai);
    if (ret != 0) {
        return nil;
    }
    int family = ai->ai_family;

    void *addr;
    if (family == AF_INET6) {
        addr = &((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr;
    } else {
        addr = &((struct sockaddr_in *)ai->ai_addr)->sin_addr;
    }
    char buf[64] = {0};
    const char *ip = inet_ntop(family, addr, buf, sizeof(buf));
#ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
    if (![QNIP isIpV6FullySupported] && family == AF_INET) {
        char buf2[64] = {0};
        ret = qn_local_ip_internal(buf2, sizeof(buf2), "64:ff9b::808:808");
        if (strchr(buf2, ':') != NULL) {
            bzero(buf, sizeof(buf));
            qn_nat64(buf, sizeof(buf), *((uint32_t *)addr));
        }
    }
#endif

    freeaddrinfo(ai);
    return [NSString stringWithUTF8String:ip];
}

+ (NSString *)local {
    char buf[64] = {0};
    int err = qn_localIp(buf, sizeof(buf));
    if (err != 0) {
        return nil;
    }
    return [NSString stringWithUTF8String:buf];
}

+ (NSString *)ipHost:(NSString *)ip {
    NSRange range = [ip rangeOfString:@":"];
    if (range.location != NSNotFound) {
        return [NSString stringWithFormat:@"[%@]", ip];
    }
    return ip;
}

+ (NSString *)nat64:(NSString *)ip {
    struct in_addr s = {0};
    inet_pton(AF_INET, ip.UTF8String, (void *)&s);
    char buf[64] = {0};
    qn_nat64(buf, sizeof(buf), (uint32_t)s.s_addr);
    return [NSString stringWithUTF8String:buf];
}

+ (BOOL)isIpV6FullySupported {
#if defined(__IPHONE_OS_VERSION_MAX_ALLOWED)
    float sysVersion = [[[UIDevice currentDevice] systemVersion] floatValue];
    if (sysVersion < 9.0) {
        return NO;
    }
#else
    NSOperatingSystemVersion sysVersion = [[NSProcessInfo processInfo] operatingSystemVersion];
    if (sysVersion.majorVersion < 10) {
        return NO;
    } else if (sysVersion.majorVersion == 10) {
        return sysVersion.minorVersion >= 11;
    }
#endif
    return YES;
}

+ (BOOL)mayBeIpV4:(NSString *)domain {
    NSUInteger l = domain.length;
    if (l > 15 || l < 7) {
        return NO;
    }
    const char *str = domain.UTF8String;
    if (str == nil) {
        return NO;
    }

    for (const char *p = str; p < str + l; p++) {
        if ((*p < '0' || *p > '9') && *p != '.') {
            return NO;
        }
    }
    return YES;
}

@end
