//
//  PLPlayerTypeDefines.h
//  PLPlayerKit
//
//  Created on 15/5/6.
//  Copyright (c) 2015年 Pili Engineering. All rights reserved.
//

#ifndef PLPlayerKit_PLPlayerTypeDefines_h
#define PLPlayerKit_PLPlayerTypeDefines_h

extern NSString * const PLMovieParameterMinBufferedDuration;    // Float, default 0.2s for local video, 2.0s for streaming video
extern NSString * const PLMovieParameterMaxBufferedDuration;    // Float, default 0.4s for local video, 4.0s for streaming video
extern NSString * const PLMovieParameterDisableDeinterlacing;   // BOOL
extern NSString * const PLMovieParameterFrameViewContentMode;   // default as UIViewContentModeScaleAspectFit.

extern NSString * const PLPlayerErrorDomain;

typedef NS_ENUM(NSUInteger, PLPlayerError) {
    PLPlayerErrorNone,
    PLPlayerErrorOpenFile,
    PLPlayerErrorStreamInfoNotFound,
    PLPlayerErrorStreamNotFound,
    PLPlayerErrorCodecNotFound,
    PLPlayerErrorOpenCodec,
    PLPlayerErrorAllocateFrame,
    PLPlayerErroSetupScaler,
    PLPlayerErroReSampler,
    PLPlayerErroUnsupported
};

#endif
