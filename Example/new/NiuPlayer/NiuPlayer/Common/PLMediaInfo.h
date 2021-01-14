//
//  PLMediaInfo.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/7.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PLMediaInfo : NSObject

@property (nonatomic, strong) NSString  *mimeType;

@property (nonatomic, assign) long long putTime;

@property (nonatomic, assign) NSInteger type;

@property (nonatomic, assign) long long fileSize;

@property (nonatomic, strong) NSString *mediaHash;

@property (nonatomic, strong) NSString *headerImg;

@property (nonatomic, strong) NSString *thumbURL;

@property (nonatomic, strong) NSString *videoURL;

@property (nonatomic, strong) NSString *endUser;

@property (nonatomic, strong) NSString *detailDesc;

@property (nonatomic, strong) NSString *name;

@end
