//
//  PLFrame.h
//  PLPlayerKit
//
//  Created by liang on 8/24/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PLPKFrame : NSObject

@property (nonatomic, assign) SInt64 timestamp;
@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (nonatomic, strong) NSData *luma;
@property (nonatomic, strong) NSData *chromaB;
@property (nonatomic, strong) NSData *chromaR;

@property (nonatomic, strong) NSData *samples;

- (instancetype)initWithData:(uint8_t **)data
                    lineSize:(int *)linesize
                      planar:(int)planar
                       width:(int)width
                      height:(int)height;

- (instancetype)initWithData:(uint8_t *)data size:(size_t)size;

@end
