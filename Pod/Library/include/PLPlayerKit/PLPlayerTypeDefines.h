//
//  PLPlayerTypeDefines.h
//  PLPlayerKit
//
//  Created on 15/5/6.
//  Copyright (c) 2015年 Pili Engineering. All rights reserved.
//

#ifndef PLPlayerKit_PLPlayerTypeDefines_h
#define PLPlayerKit_PLPlayerTypeDefines_h

/**
 * Parameters Key
 */
extern NSString * const PLMovieParameterMinBufferedDuration;    // Float, default 0.2s for local video, 2.0s for streaming video
extern NSString * const PLMovieParameterMaxBufferedDuration;    // Float, default 0.4s for local video, 4.0s for streaming video
extern NSString * const PLMovieParameterDisableDeinterlacing;   // BOOL
extern NSString * const PLMovieParameterFrameViewContentMode;   // default as UIViewContentModeScaleAspectFit.
extern NSString * const PLMovidParameterAutoPlayEnable;         // BOOL, default as NO

/**
 * Notifications
 */
extern NSString *PLAudioSessionInterruptionStateKey;
extern NSString *PLAudioSessionInterruptionTypeKey;
extern NSString *PLAudioSessionDidInterrupteNotification;

extern NSString *PLAudioSessionRouteChangeReasonKey;
extern NSString *PLAudioSessionRouteDidChangeNotification;

extern NSString *PLAudioSessionCurrentHardwareOutputVolumeKey;
extern NSString *PLAudioSessionCurrentHardwareOutputVolumeDidChangeNotification;

/**
 * Error
 */
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

/**
 * Player State
 */
typedef NS_ENUM(NSUInteger, PLVideoPlayerState) {
    PLVideoPlayerStateUnknow = 0,
    PLVideoPlayerStatePreparing,
    PLVideoPlayerStateReady,
    PLVideoPlayerStateCaching,
    PLVideoPlayerStatePlaying,
    PLVideoPlayerStatePaused,
    PLVideoPlayerStateEnded
};

/**
 * Others
 */
typedef NS_ENUM(NSUInteger, PLVideoPlayerBackgroundMode) {
    PLVideoPlayerBackgroundModeAutoPause
};

#endif
