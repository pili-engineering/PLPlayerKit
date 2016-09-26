//
//  PLPKPacket.h
//  PLPlayerKit
//
//  Created by liang on 8/23/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSUInteger, PLPacketType) {
    PLPacketNone,
    PLPacketAudio,
    PLPacketVideo,
};

@class PLFLVTag;

@interface PLPKPacket : NSObject

@property (nonatomic, readonly) PLFLVTag *flvTag;

@property (nonatomic, assign) uint8_t *data;
@property (nonatomic, assign) size_t size;

@property (nonatomic, assign) PLPacketType type;

@property (nonatomic, assign) SInt64 pts;
@property (nonatomic, assign) SInt64 dts;

@property (nonatomic, assign) BOOL isKeyFrame;
@property (nonatomic, assign) BOOL isSequence;

- (instancetype)initWithFLVData:(const uint8_t *)data size:(size_t)size;

@end
