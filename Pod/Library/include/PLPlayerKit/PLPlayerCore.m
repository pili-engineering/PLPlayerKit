//
//  PLPlayerCore.m
//  PLPlayerKit
//
//  Created by liang on 8/19/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#include "libavformat/avformat.h"
#include "rtmp.h"

#import "PLPlayerCore.h"
#import "PLPlayerEnv.h"

#import "PLLibRTMPStream.h"
#import "PLFLVDemux.h"
#import "PLAVDemux.h"
#import "PLAVAudioCodec.h"
#import "PLAVVideoCodec.h"


@implementation PLPlayerCore

+ (void)load {
    [PLPlayerCore registerAll];
}

+ (void)registerAll {
    av_register_all();
    avformat_network_init();
    NSLog(@"ffmpeg build configure: %s", avcodec_configuration());
    
    [PLPlayerEnv setFFmpegVersion:av_version_info()];
    [PLPlayerEnv setLibRTMPVersion:PILI_RTMP_Version()];
    
    [PLPlayerCore registerNetwork];
    [PLPlayerCore registerDemux];
    [PLPlayerCore registerCodec];
}

+ (void)registerNetwork {
    [PLURLStream registerURLStreamFactory:^PLURLStream *(NSURL *url) {
        return [[PLLibRTMPStream alloc] initWithURL:url];
    }];
}

+ (void)registerDemux {
    [PLDemux registerDemuxFactory:^PLDemux *(PLURLStream *stream) {
        if (stream) {
            NSURL *url = stream.url;
            NSString *scheme = url.scheme;
            
            if ([scheme isEqualToString:@"rtmp"]) {
                return [[PLFLVDemux alloc] initWithStream:stream];
            } else {
                return [[PLAVDemux alloc] initWithStream:stream];
            }
        }
        
        return nil;
    }];
}

+ (void)registerCodec {
    [PLAudioCodec registerCodecFactory:^PLAudioCodec *(PLDemux *demux) {
        return [PLAVAudioCodec new];
    }];
    
    [PLVideoCodec registerCodecFactory:^PLVideoCodec *(PLDemux *demux) {
        return [PLAVVideoCodec new];
    }];
}

@end
