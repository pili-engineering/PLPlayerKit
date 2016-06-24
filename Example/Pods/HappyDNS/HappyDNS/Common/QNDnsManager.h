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

@protocol QNIpSorter <NSObject>
- (NSArray *)sort:(NSArray *)ips;
@end

@interface QNDnsManager : NSObject
- (NSArray *)query:(NSString *)domain;
- (NSArray *)queryWithDomain:(QNDomain *)domain;
- (void)onNetworkChange:(QNNetworkInfo *)netInfo;
- (instancetype)init:(NSArray *)resolvers networkInfo:(QNNetworkInfo *)netInfo;
- (instancetype)init:(NSArray *)resolvers networkInfo:(QNNetworkInfo *)netInfo sorter:(id<QNIpSorter>)sorter;
- (instancetype)putHosts:(NSString *)domain ip:(NSString *)ip;
- (instancetype)putHosts:(NSString *)domain ip:(NSString *)ip provider:(int)provider;
@end

@interface QNDnsManager (NSURL)
- (NSURL *)queryAndReplaceWithIP:(NSURL *)url;
@end