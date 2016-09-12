//
//  PLAVPacket.m
//  PLPlayerKit
//
//  Created by liang on 8/24/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import "PLAVPacket.h"

@implementation PLAVPacket

- (instancetype)initWithAVPacket:(AVPacket *)avpacket {
    self = [super init];
    if (self) {
        _avpacket = avpacket;
        self.isKeyFrame = avpacket->flags & AV_PKT_FLAG_KEY;
        self.isSequence = NO;
        self.data = avpacket->data;
        self.size = avpacket->size;
        self.pts = avpacket->pts;
        self.dts = avpacket->dts;
    }
    
    return self;
}

-(void)dealloc{
    if (_avpacket) {
        av_packet_unref(_avpacket);
        av_free(_avpacket);
        _avpacket = NULL;
    }
    
    self.data = NULL;
    self.size = 0;
}

@end
