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
extern NSString * const PLPlayerParameterMinBufferedDuration;    // Float, default 0.2s for local video, 2.0s for streaming video
extern NSString * const PLPlayerParameterMaxBufferedDuration;    // Float, default 0.4s for local video, 4.0s for streaming video
extern NSString * const PLPlayerParameterAutoPlayEnable;         // BOOL, default as NO

extern NSString * const PLVideoParameterDisableDeinterlacing;   // BOOL
extern NSString * const PLVideoParameterFrameViewContentMode;   // default as UIViewContentModeScaleAspectFit.

extern NSString * const PLMovieParameterMinBufferedDuration DEPRECATED_ATTRIBUTE;    // Float, default 0.2s for local video, 2.0s for streaming video
extern NSString * const PLMovieParameterMaxBufferedDuration DEPRECATED_ATTRIBUTE;    // Float, default 0.4s for local video, 4.0s for streaming video
extern NSString * const PLMovieParameterDisableDeinterlacing DEPRECATED_ATTRIBUTE;   // BOOL
extern NSString * const PLMovieParameterFrameViewContentMode DEPRECATED_ATTRIBUTE;   // default as UIViewContentModeScaleAspectFit.
extern NSString * const PLMovieParameterAutoPlayEnable DEPRECATED_ATTRIBUTE;         // BOOL, default as NO

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
typedef NS_ENUM(NSUInteger, PLPlayerState) {
    PLPlayerStateUnknow = 0,
    PLPlayerStatePreparing,
    PLPlayerStateReady,
    PLPlayerStateCaching,
    PLPlayerStatePlaying,
    PLPlayerStatePaused,
    PLPlayerStateEnded
};

typedef NS_ENUM(NSUInteger, PLVideoPlayerState) {
    PLVideoPlayerStateUnknow = PLPlayerStateUnknow,
    PLVideoPlayerStatePreparing = PLPlayerStatePreparing,
    PLVideoPlayerStateReady = PLPlayerStateReady,
    PLVideoPlayerStateCaching = PLPlayerStateCaching,
    PLVideoPlayerStatePlaying = PLPlayerStatePlaying,
    PLVideoPlayerStatePaused = PLPlayerStatePaused,
    PLVideoPlayerStateEnded = PLPlayerStateEnded
} DEPRECATED_ATTRIBUTE;

/**
 * Others
 */
typedef NS_ENUM(NSUInteger, PLPlayerBackgroundMode) {
    PLPlayerBackgroundModeAutoPause
};

#endif
