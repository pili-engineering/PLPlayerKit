//
//  PLAVPacket.h
//  PLPlayerKit
//
//  Created by liang on 8/24/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import "libavformat/avformat.h"

#import "PLPKPacket.h"

@interface PLAVPacket : PLPKPacket

@property (nonatomic, readonly) AVPacket *avpacket;

- (instancetype)initWithAVPacket:(AVPacket *)avpacket;

@end
