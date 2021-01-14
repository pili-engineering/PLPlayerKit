//
//  PLHttpSession.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/7.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLHttpSession.h"

#define REQUEST_HOST    @"http://api-demo.qnsdk.com"
#define PLAY_HOST       @"http://demo-videos.qnsdk.com"

@implementation PLHttpSession

//    @"http://api-demo.qnsdk.com/v1/kodo/bucket/demo-videos?prefix=shortvideo"
+ (void)requestShortMediaList:(void (^)(NSArray *, NSError *))completeBlock {
    
    NSString *urlString = [REQUEST_HOST stringByAppendingPathComponent:@"v1/kodo/bucket/demo-videos?prefix=shortvideo"];
    
    NSURL *url = [NSURL URLWithString:urlString];
    [PLHttpSession reuqestURL:url complete:^(NSData *data, NSError *error) {
        
        NSArray *array = nil;
        if (data) {
            NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableLeaves error:&error];
            array = [PLHttpSession mediaWithDic:dic];
        }
        dispatch_main_async_safe(^{
            completeBlock(array, error);
        })
    }];
}

//    @"http://api-demo.qnsdk.com/v1/kodo/bucket/demo-videos?prefix=movies"
+ (void)requestLongMediaList:(void (^)(NSArray *, NSError *))completeBlock {
    
    NSString *urlString = [REQUEST_HOST stringByAppendingPathComponent:@"v1/kodo/bucket/demo-videos?prefix=movies"];
    NSURL* url = [NSURL URLWithString:urlString];
    [PLHttpSession reuqestURL:url complete:^(NSData *data, NSError *error) {
        
        NSArray *array = nil;
        if (data) {
            NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableLeaves error:&error];
            array = [PLHttpSession mediaWithDic:dic];
        }
        dispatch_main_async_safe(^{
            completeBlock(array, error);
        })
    }];
}

+ (void)requestLiveMediaList:(void (^)(NSArray *, NSError *))completeBlock {
    
    NSString *urlString = [REQUEST_HOST stringByAppendingPathComponent:@"/v1/live/streams/live"];
//    NSString *urlString = [REQUEST_HOST stringByAppendingPathComponent:@"/v1/streams/live"];
    NSURL* url = [NSURL URLWithString:urlString];
    
    [PLHttpSession reuqestURL:url complete:^(NSData *data, NSError *error) {
        
        NSArray *array = nil;
        if (data) {
            NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableLeaves error:&error];
            array = [PLHttpSession mediaWithDic:dic];
        }
        dispatch_main_async_safe(^{
            completeBlock(array, error);
        })
    }];
}

+ (NSArray *)mediaWithDic:(NSDictionary *)dic {
    
    NSMutableArray *mediaArray = [[NSMutableArray alloc] init];
    
    static NSString *headerImage[] = {
        @"0.jpg",
        @"1.jpg",
        @"2.jpeg",
        @"3.jpg",
        @"4.jpg",
        @"5.jpeg",
    };
    
    do {
        if (![dic isKindOfClass:[NSDictionary class]]) break;
        
        NSArray *itemArray = [dic objectForKey:@"Items"];
        if (![itemArray isKindOfClass:[NSArray class]]) break;
        
        for (NSDictionary *dic in itemArray) {
            if (![dic isKindOfClass:[NSDictionary class]]) {
                continue;
            }
            PLMediaInfo *media= [[PLMediaInfo alloc] init];
            
            NSString *key = [dic objectForKey:@"key"];
            NSArray *tempArray = [key componentsSeparatedByString:@"/"];
            NSString *videoName = [tempArray lastObject];
            NSString *name = [[videoName componentsSeparatedByString:@"."] firstObject];
            
            media.mediaHash = [dic objectForKey:@"hash"];
            media.videoURL = [NSString stringWithFormat:@"%@/%@", PLAY_HOST, key];
            media.thumbURL = [NSString stringWithFormat:@"%@/snapshoot/%@.jpg", PLAY_HOST, name];
            media.fileSize = [[dic objectForKey:@"fsize"] longLongValue];
            media.mimeType = [dic objectForKey:@"mimeType"];
            media.type     = [[dic objectForKey:@"type"] integerValue];
            media.endUser  = [dic objectForKey:@"endUser"];
            media.putTime  = [[dic objectForKey:@"putTime"] longLongValue];
            
            media.videoURL = [media.videoURL stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
            media.thumbURL = [media.thumbURL stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
            
            media.headerImg = @"qiniu";
            media.name = @"七牛云 简单·可信赖";
            
            media.detailDesc = [NSDate yyyyMMddStringWithSecond:media.putTime / 10000000];
            
            [mediaArray addObject:media];
        }
    } while (0);
    
    return mediaArray;
}

+ (void)reuqestURL:(NSURL *)url complete:(void(^)(NSData *data, NSError *error))completeBlock {
    
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
    request.HTTPMethod = @"GET";
    request.timeoutInterval = 10;
    
    NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        completeBlock(data, error);
    }];
    [task resume];
}

@end
