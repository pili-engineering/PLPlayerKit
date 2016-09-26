//
//  PLAVAudioCodec.m
//  PLPlayerKit
//
//  Created by liang on 8/25/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <CoreAudio/CoreAudioTypes.h>

#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"

#import "PLAVAudioCodec.h"


void adjustAudioFramePTS(AVFrame *frame, AVCodecContext *avctx) {
    AVRational tb = (AVRational){1, frame->sample_rate};
    if (frame->pts != AV_NOPTS_VALUE) {
        frame->pts = av_rescale_q(frame->pts, avctx->time_base, tb);
    } else if (frame->pkt_pts != AV_NOPTS_VALUE) {
        frame->pts = av_rescale_q(frame->pkt_pts, av_codec_get_pkt_timebase(avctx), tb);
    }
}

@interface PLAVAudioCodec ()
{
    AVCodec *_avCodec;
    AVCodecContext *_avCodecContext;
    AVPacket _avPacket;
    AVFrame *_avFrame;
    SwrContext *_swrContext;
}

@property (nonatomic, assign) BOOL needFreeContext;

@end

@implementation PLAVAudioCodec

- (BOOL)openWithContext:(void *)context {
    [self close];
    _avCodecContext = context;
    if (_avCodecContext) {
        _needFreeContext = NO;
        _avCodec = avcodec_find_decoder(_avCodecContext->codec_id);
        if (_avCodec) {
            if (avcodec_open2(_avCodecContext, _avCodec, NULL) == 0) {
                _avFrame = av_frame_alloc();
                av_init_packet(&_avPacket);
                return YES;
            }
        }
    }
    
    return NO;
}

- (BOOL)openWithAudioStreamBasicDescription:(AudioStreamBasicDescription)asbd {
    [self close];
    _avCodec = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (_avCodec) {
        _avCodecContext = avcodec_alloc_context3(_avCodec);
        if (_avCodecContext) {
            _needFreeContext = YES;
            _avCodecContext->sample_rate = asbd.mSampleRate;
            _avCodecContext->channels = asbd.mChannelsPerFrame;
            if (avcodec_open2(_avCodecContext, _avCodec, NULL) == 0) {
                _avFrame = av_frame_alloc();
                av_init_packet(&_avPacket);
                return YES;
            }
        }
    }
    return NO;
}

- (void)dealloc {
    [self close];
}

- (BOOL)close {
    NSLog(@"close audio codec");
    if (_avCodecContext) {
        avcodec_close(_avCodecContext);
        if (self.needFreeContext) {
            avcodec_free_context(&_avCodecContext);
        }
        _avCodecContext = NULL;
    }
    
    if (_avFrame) {
        av_frame_free(&_avFrame);
    }
    
    if (_swrContext) {
        swr_close(_swrContext);
        swr_free(&_swrContext);
        _swrContext = NULL;
    }
    
    return YES;
}

- (void)updateAudioInfo {
    if (_swrContext) {
        return;
    }
    
    _swrContext = swr_alloc();
    swr_alloc_set_opts(_swrContext,
                       _avCodecContext->channel_layout,
                       AV_SAMPLE_FMT_FLT,
                       _avCodecContext->sample_rate,
                       _avCodecContext->channel_layout,
                       _avCodecContext->sample_fmt,
                       _avCodecContext->sample_rate,0, NULL);
    swr_init(_swrContext);
    
    AudioStreamBasicDescription asbd = {0};
    asbd.mSampleRate = _avCodecContext->sample_rate;
    asbd.mChannelsPerFrame = _avCodecContext->channels;
    if ([self.delegate respondsToSelector:@selector(audioCodec:AudioDescription:frameSize:)]) {
        [self.delegate audioCodec:self AudioDescription:asbd frameSize:_avCodecContext->frame_size];
    }
}

- (PLPKFrame *)decode:(PLPKPacket *)packet {
    _avPacket.data = packet.data;
    _avPacket.size = packet.size;
    _avPacket.pts = packet.pts;
    
    int got_frame = 0;
    int ret = avcodec_decode_audio4(_avCodecContext, _avFrame, &got_frame, &_avPacket);
    if (ret < 0)
    {
        NSLog(@"Error decoding audio frame (%d)", ret);
    }
    
    if (got_frame) {
        if (_swrContext == NULL) {
            [self updateAudioInfo];
        }
        
        int out_linesize;
        int out_buffer_size = av_samples_get_buffer_size(&out_linesize,
                                                         _avCodecContext->channels,
                                                         _avCodecContext->frame_size,
                                                         AV_SAMPLE_FMT_FLT, 1);
        uint8_t *out_buffer = malloc(out_buffer_size);
        swr_convert(_swrContext,
                    &out_buffer,
                    out_linesize,
                    (const uint8_t **)_avFrame->extended_data,
                    _avFrame->nb_samples);
        
        PLPKFrame *frame = [[PLPKFrame alloc] initWithData:out_buffer size:out_buffer_size];
        frame.timestamp = packet.pts;
        
        if (self.needFreeContext) {
            frame.timestamp = packet.pts;
        } else {
            adjustAudioFramePTS(_avFrame, _avCodecContext);
            AVRational tb = (AVRational){1, _avFrame->sample_rate};
            frame.timestamp = _avFrame->pts * av_q2d(tb) * 1000;
        }
        
        if (out_buffer) {
            free(out_buffer);
        }
        
        av_frame_unref(_avFrame);
        return frame;
    }
    return nil;
}

@end
