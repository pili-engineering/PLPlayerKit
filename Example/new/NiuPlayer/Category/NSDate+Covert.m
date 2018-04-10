//
//  NSDate+Covert.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/14.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "NSDate+Covert.h"

@implementation NSDate (Covert)

+ (NSString *)yyyyMMddStringWithSecond:(NSTimeInterval)time {
    
    NSDate *date = [NSDate dateWithTimeIntervalSince1970:time];
    NSDateFormatter *dateFormat = [[NSDateFormatter alloc] init];
    [dateFormat setDateFormat:@"yyyy-MM-dd"];
    
    return [dateFormat stringFromDate:date];
}

@end
