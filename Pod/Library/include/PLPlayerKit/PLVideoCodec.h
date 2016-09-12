//
//  PLVideoCodec.h
//  PLPlayerKit
//
//  Created by liang on 8/23/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "PLPKPacket.h"
#import "PLPKFrame.h"

@class PLVideoCodec;
@class PLDemux;

typedef PLVideoCodec *(^VideoCodecFactory)(PLDemux *demux);

@interface PLVideoCodec : NSObject

@property (nonatomic, readonly) uint8_t *extraData;
@property (nonatomic, readonly) size_t extraSize;
@property (nonatomic, readonly) double timebase;
@property (nonatomic, readonly) BOOL isH264;

+ (instancetype)createCodecFactory:(PLDemux *)demux;
+ (void)registerCodecFactory:(VideoCodecFactory)codecFactory;

- (BOOL)open;
- (BOOL)openWithContext:(void *)context stream:(void *)stream;
- (BOOL)close;

- (PLPKFrame *)decode:(PLPKPacket *)packet;


@end
