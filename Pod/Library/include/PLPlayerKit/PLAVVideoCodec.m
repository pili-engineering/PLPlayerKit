//
//  PLAVVideoCodec.m
//  PLPlayerKit
//
//  Created by liang on 8/24/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

#import "PLAVVideoCodec.h"

@interface PLAVVideoCodec ()
{
    AVStream *_avStream;
    AVCodecContext *_avCodecContext;
    AVCodec *_avCodec;
    AVPacket _packet;
    AVFrame *_frame;
    double _timebase;
    BOOL _isH264;
}

@property (nonatomic, assign) BOOL needFreeContext;

@end

@implementation PLAVVideoCodec

- (void)dealloc {
    [self close];
}

- (BOOL)open {
    [self close];
    
    _avCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(_avCodec) {
        _avCodecContext = avcodec_alloc_context3(_avCodec);
        self.needFreeContext = YES;
        if (_avCodecContext) {
            if (_avCodec->capabilities & AV_CODEC_CAP_TRUNCATED)
                _avCodecContext->flags |= AV_CODEC_FLAG_TRUNCATED;
            
            _frame = av_frame_alloc();
            av_init_packet(&_packet);
            if (avcodec_open2(_avCodecContext, _avCodec, NULL) == 0) {
                _timebase = 1;
                _isH264 = YES;
                return YES;
            }
        }
    }
    
    return NO;
}


- (BOOL)openWithContext:(void *)context stream:(void *)stream {
    [self close];
    
    _avStream = stream;
    _avCodecContext = context;
    self.needFreeContext = NO;
    
    if (_avCodecContext) {
        _avCodec = avcodec_find_decoder(_avCodecContext->codec_id);
        if (_avCodec) {
            _frame = av_frame_alloc();
            av_init_packet(&_packet);
            if (avcodec_open2(_avCodecContext, _avCodec, NULL) == 0) {
                if (_avStream) {
                    _timebase = av_q2d(_avStream->time_base) * 1000;
                } else {
                    _timebase = 1;
                }
                
                if (_avCodecContext->codec_id == AV_CODEC_ID_H264) {
                    _isH264 = YES;
                } else {
                    _isH264 = NO;
                }
                return YES;
            }
        }
    }
    
    return NO;
}

- (BOOL)close {
    if (_avCodecContext) {
        avcodec_close(_avCodecContext);
        
        if (self.needFreeContext) {
            av_free(_avCodecContext);
        }
        _avCodecContext = NULL;
    }
    
    if (_frame) {
        av_frame_free(&_frame);
        _frame = NULL;
    }
    
    _avCodec = NULL;
    
    return YES;
}

- (PLPKFrame *)decode:(PLPKPacket *)packet {
    _packet.pos = -1;
    _packet.dts = packet.dts;
    _packet.pts = packet.pts;
    _packet.data = packet.data;
    _packet.size = packet.size;
    int len, got_frame;
    len = avcodec_decode_video2(_avCodecContext, _frame, &got_frame, &_packet);
    
    if (len < 0) {
        NSLog(@"soft decoder fail (%d)", len);
    }
    if (got_frame) {
        PLPKFrame *frame = [[PLPKFrame alloc] initWithData:_frame->data
                                              lineSize:_frame->linesize
                                                planar:3
                                                 width:_frame->width
                                                height:_frame->height];
        if (_avStream) {
            frame.timestamp = _frame->pts == AV_NOPTS_VALUE ? packet.dts  : _frame->pts;
            AVRational tb = _avStream->time_base;
            frame.timestamp = frame.timestamp * av_q2d(tb) * 1000;
        } else {
            frame.timestamp = packet.dts;
        }
        
        av_frame_unref(_frame);
        
        return frame;
    }
    
    return nil;
}

#pragma mark -- getter/setter

- (uint8_t *)extraData {
    if (_avCodecContext) {
        return _avCodecContext->extradata;
    }
    
    return NULL;
}

- (size_t)extraSize {
    if (_avCodecContext) {
        return _avCodecContext->extradata_size;
    }
    
    return 0;
}

- (double)timebase {
    return _timebase;
}

- (BOOL)isH264 {
    return _isH264;
}

@end
