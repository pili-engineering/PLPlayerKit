//
//  PLAVDemux.m
//  PLPlayerKitDemo
//
//  Created by liang on 8/24/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#include "libavformat/avformat.h"

#import "NSError+Pili.h"
#import "PLAVDemux.h"
#import "PLAVPacket.h"

// the interrut_callback 每隔一段时间会回调一次，无论是否出错，主要是提供了一个内部打断avio的办法。
static int read_interrupt_cb(void *ctx) {
    if (ctx == NULL) {
        return 1;
    }
    PLAVDemux* self = (__bridge PLAVDemux*)ctx;
    
    if ([self.delegate respondsToSelector:@selector(IOInterruptDemux:)]) {
        return [self.delegate IOInterruptDemux:self];
    } else {
        return 0;
    }
}

@interface PLAVDemux ()
{
    AVFormatContext *_formatContext;
    AVDictionary *_option;
}

@property (nonatomic, assign) int audioIndex;
@property (nonatomic, assign) int videoIndex;

@end

@implementation PLAVDemux

- (void)dealloc {
    av_dict_free(&_option);
}

- (BOOL)open:(NSError * __autoreleasing *)error {
    avformat_close_input(&(_formatContext));
    _formatContext = avformat_alloc_context();
    av_dict_set_int(&_option, "probesize", 100*1024, 0);
    _formatContext->interrupt_callback.callback = read_interrupt_cb;
    _formatContext->interrupt_callback.opaque = (__bridge void *)(self);
    
    const char *url = [self.stream.url.absoluteString cStringUsingEncoding:NSUTF8StringEncoding];
    
    int ret;
    if((ret = avformat_open_input(&_formatContext, url, NULL, &_option)) != 0) {
        NSLog(@"Couldn't open input stream %d %s.\n", ret, av_err2str(ret));
        if (error) {
            *error = [NSError errorWithPlayerError:PLPlayerErrorStreamFormatNotSupport
                                           message:av_err2str(ret)];
        }
        return NO;
    }
    
    av_format_inject_global_side_data(_formatContext);
    
    if((ret = avformat_find_stream_info(_formatContext,NULL))<0){
        NSLog(@"Couldn't find stream information %d %s.\n", ret, av_err2str(ret));
        if (error) {
            *error = [NSError errorWithPlayerError:PLPlayerErrorStreamFormatNotSupport
                                           message:av_err2str(ret)];
        }
        return NO;
    }
    
    av_dump_format(_formatContext, 0, url, 0);
    
    for(int i=0; i<_formatContext->nb_streams; i++){
        int t = _formatContext->streams[i]->codec->codec_type;
        if (t == AVMEDIA_TYPE_AUDIO) {
            _audioIndex = i;
            self.audioContext = _formatContext->streams[i]->codec;
            self.audioStream = _formatContext->streams[i];
        } else if (t == AVMEDIA_TYPE_VIDEO) {
            _videoIndex=i;
            self.videoContext = _formatContext->streams[i]->codec;
            self.videoStream = _formatContext->streams[i];
        }
    }
    
    return YES;
}

- (BOOL)close {
    avformat_close_input(&(_formatContext));
    _formatContext = NULL;
    return YES;
}

- (void)seekTo:(CMTime)time {
    int64_t seekPos = CMTimeGetSeconds(time);
    seekPos = av_rescale(seekPos, AV_TIME_BASE, 1);
    int64_t seekMin = INT64_MIN;
    int64_t seekMax = INT64_MAX;
    avformat_seek_file(_formatContext, -1, seekMin, seekPos, seekMax, 0);
}

- (CMTime)duration {
    if (_formatContext && _formatContext->duration != AV_NOPTS_VALUE) {
        return CMTimeMake(_formatContext->duration, AV_TIME_BASE);
    } else {
        return CMTimeMake(0, 1);
    }
}

- (CMTime)startTime {
    if (_formatContext) {
        int64_t start_time = _formatContext->start_time;
        if (start_time > 0 && start_time != AV_NOPTS_VALUE) {
//            start_time = av_rescale(start_time, 1, AV_TIME_BASE);
            return CMTimeMake(start_time, AV_TIME_BASE);
        } else {
            return CMTimeMake(0, 1);
        }
    } else {
        return CMTimeMake(0, 1);
    }
}

- (PLPKPacket *)readPacket:(NSError *__autoreleasing *)error {
    AVPacket *avpacket=(AVPacket *)av_malloc(sizeof(AVPacket));
    
    int ret = av_read_frame(_formatContext, avpacket);
    
    if (ret >= 0) {
        PLPKPacket *packet;
        if (avpacket->stream_index == _audioIndex) {
            packet = [[PLAVPacket alloc] initWithAVPacket:avpacket];
            packet.type = PLPacketAudio;
        } else if (avpacket->stream_index == _videoIndex){
            packet = [[PLAVPacket alloc] initWithAVPacket:avpacket];
            packet.type = PLPacketVideo;
        } else {
            packet = [PLAVPacket new];
            packet.type = PLPacketNone;
        }
        
        return packet;
    } else {
        av_free(avpacket);
        NSLog(@"read frame failed %d", ret);
        if (ret == AVERROR_EOF) {
            ret = PLPlayerErrorEOF;
        }
        *error = [NSError errorWithDomain:PLPlayerErrorDomain code:ret userInfo:nil];
        return nil;
    }
}

@end
