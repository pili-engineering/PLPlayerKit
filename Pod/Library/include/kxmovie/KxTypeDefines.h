//
//  KxTypeDefines.h
//  kxmovie
//
//  Created by 0day on 15/7/17.
//
//

#ifndef kxmovie_KxTypeDefines_h
#define kxmovie_KxTypeDefines_h

extern NSString * const KxPlayerParameterMinBufferedDuration;    // Float, default as 2.0s for network and 0.2s for local
extern NSString * const KxPlayerParameterMaxBufferedDuration;    // Float, default as 4.0s for network and 0.4s for local
extern NSString * const KxPlayerParameterAutoPlayEnable;         // BOOL, default as NO

extern NSString * const KxMovieParameterDisableDeinterlacing;   // BOOL
extern NSString * const KxMovieParameterFrameViewContentMode;   // default as UIViewContentModeScaleAspectFit.


typedef NS_ENUM(NSUInteger, KxPlayerState) {
    KxPlayerStateStopped = 0,
    KxPlayerStatePreparing,
    KxPlayerStateReady,
    KxPlayerStateCaching,
    KxPlayerStatePlaying,
    KxPlayerStatePaused
};

#endif
