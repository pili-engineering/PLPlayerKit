//
//  QNDnsManager.h
//  HappyDNS
//
//  Created by bailong on 15/6/23.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import <Foundation/Foundation.h>

@class QNNetworkInfo;
@class QNDomain;

/**
 *    getaddrinfo 回调上层的函数
 *
 *    @param host    请求的域名
 *    @return        ip 列表
 */
typedef NSArray * (^QNGetAddrInfoCallback)(NSString *host);

/**
 *    ip status 回调上层的函数
 *
 *    @param ip    请求的IP
 *    @param code  错误码
 *    @param ms    消耗时间
 */
typedef void (^QNIpStatusCallback)(NSString *ip, int code, int ms);

/**
 *    外部IP 排序接口
 */
@protocol QNIpSorter <NSObject>

/**
 *    排序方法
 *
 *    @param ips 传入的IP列表
 *
 *    @return 返回排序好的IP 列表
 */
- (NSArray *)sort:(NSArray *)ips;
@end

/**
 *    DNS请求客户端，集成了cache管理
 */
@interface QNDnsManager : NSObject

/**
 *    解析域名
 *
 *    @param domain 域名
 *
 *    @return IP列表
 */
- (NSArray *)query:(NSString *)domain;

/**
 *    解析域名，使用Domain对象进行详细约定
 *
 *    @param domain 配置了一些domain 参数的 domain 对象
 *
 *    @return IP 列表
 */
- (NSArray *)queryWithDomain:(QNDomain *)domain;

/**
 *    通知网络发生变化
 *
 *    @param netInfo 网络信息
 */
- (void)onNetworkChange:(QNNetworkInfo *)netInfo;

/**
 *    Dns client 初始化
 *
 *    @param resolvers 解析服务器列表
 *    @param netInfo   当前网络信息
 *
 *    @return DnsManager
 */
- (instancetype)init:(NSArray *)resolvers networkInfo:(QNNetworkInfo *)netInfo;

/**
 *    Dns client 初始化
 *
 *    @param resolvers 解析服务器列表
 *    @param netInfo   当前网络信息
 *    @param sorter    外部排序函数
 *
 *    @return DnsManager
 */
- (instancetype)init:(NSArray *)resolvers networkInfo:(QNNetworkInfo *)netInfo sorter:(id<QNIpSorter>)sorter;

/**
 *    内置 Hosts 解析
 *
 *    @param domain 域名
 *    @param ip     对应IP
 *
 *    @return 当前Dnsmanager, 为了链式调用
 */
- (instancetype)putHosts:(NSString *)domain ip:(NSString *)ip;

/**
 *    内置 Hosts 解析
 *
 *    @param domain 域名
 *    @param ip     对应IP
 *    @param provider 网络运营商
 *
 *    @return 当前Dnsmanager, 为了链式调用
 */
- (instancetype)putHosts:(NSString *)domain ip:(NSString *)ip provider:(int)provider;

/**
 *    设置底层 getaddrinfo 使用的回调
 *
 *    @param block 回调的代码块
 */
+ (void)setGetAddrInfoBlock:(QNGetAddrInfoCallback)block;

/**
 *    设置底层 getaddrinfo 回调使用的dnsmanager
 *
 *    @param dns 回调用的dnsmanager
 */
+ (void)setDnsManagerForGetAddrInfo:(QNDnsManager *)dns;

/**
 *    设置底层 业务统计 如connect 回调使用的Callback
 *
 *    @param block 回调返回该IP状态
 */
+ (void)setIpStatusCallback:(QNIpStatusCallback)block;

/**
 *    根据时区判断是否要设置httpDns
 *
 */
+ (BOOL)needHttpDns;

@end

/**
 *    DnsManager 的 URL 辅助类
 */
@interface QNDnsManager (NSURL)

/**
 *    使用URL 进行请求
 *
 *    @param url 请求的Url
 *
 *    @return 返回IP 替换过的url
 */
- (NSURL *)queryAndReplaceWithIP:(NSURL *)url;
@end
