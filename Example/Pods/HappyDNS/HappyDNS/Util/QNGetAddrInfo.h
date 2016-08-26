//
//  QNGetAddrInfo.h
//  HappyDNS
//
//  Created by bailong on 16/7/19.
//  Copyright © 2016年 Qiniu Cloud Storage. All rights reserved.
//

#ifndef QNGetAddrInfo_h
#define QNGetAddrInfo_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct qn_ips_ret {
    char *ips[1];
} qn_ips_ret;

typedef qn_ips_ret *(*qn_dns_callback)(const char *host);

typedef void (*qn_ip_report_callback)(const char *ip, int code, int time_ms);

extern void qn_free_ips_ret(qn_ips_ret *ip_list);

extern int qn_getaddrinfo(const char *hostname, const char *servname, const struct addrinfo *hints, struct addrinfo **res);

extern void qn_freeaddrinfo(struct addrinfo *ai);

extern void qn_set_dns_callback(qn_dns_callback cb);

extern void qn_set_ip_report_callback(qn_ip_report_callback cb);

extern void qn_ip_report(const struct addrinfo *info, int code, int time_ms);

#ifdef __cplusplus
};
#endif

#endif /* QNGetAddrInfo_h */
