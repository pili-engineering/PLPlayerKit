//
//  KxMovieController.h
//  kxmovie
//
//  Created by 0day on 15/5/6.
//
//

#import <Foundation/Foundation.h>

@class KxMovieDecoder;

extern NSString * const KxMovieParameterMinBufferedDuration;    // Float, default as 2.0s for network and 0.2s for local
extern NSString * const KxMovieParameterMaxBufferedDuration;    // Float, default as 4.0s for network and 0.4s for local
extern NSString * const KxMovieParameterDisableDeinterlacing;   // BOOL
extern NSString * const KxMovieParameterFrameViewContentMode;   // default as UIViewContentModeScaleAspectFit.
extern NSString * const KxMovidParameterAutoPlayEnable;         // BOOL, default as NO

typedef NS_ENUM(NSUInteger, KxMoviePlayerState) {
    KxMoviePlayerStateUnknow = 0,
    KxMoviePlayerStatePreparing,
    KxMoviePlayerStateReady,
    KxMoviePlayerStateCaching,
    KxMoviePlayerStatePlaying,
    KxMoviePlayerStatePaused,
    KxMoviePlayerStateEnded
};

@class KxMovieController;
@protocol KxMovieControllerDelegate <NSObject>

@optional
- (void)movieController:(KxMovieController *)controller playerStateDidChange:(KxMoviePlayerState)status;
- (void)movieControllerDecoderHasBeenReady:(KxMovieController *)controller;
- (void)movieController:(KxMovieController *)controller failureWithError:(NSError *)error;
- (void)movieController:(KxMovieController *)controller positionDidChange:(NSTimeInterval)position;

@end

@interface KxMovieController : NSObject

+ (id)movieControllerWithContentPath:(NSString *)path
                          parameters:(NSDictionary *)parameters;

@property (nonatomic, weak) id<KxMovieControllerDelegate> delegate;

@property (nonatomic, readonly, strong) UIView    *playerView;
@property (nonatomic, assign, getter=isUserInteractionEnable) BOOL userInteractionEnable;   // default as YES
@property (nonatomic, assign, getter=isMuted) BOOL  muted;  // default as NO

@property (nonatomic, assign, readonly) KxMoviePlayerState playerState;
@property (nonatomic, readonly, getter=isPlaying) BOOL playing;
@property (nonatomic, assign, readonly) CGFloat audioVolume;
@property (nonatomic, assign, readonly) NSTimeInterval duration;
@property (nonatomic, assign, readonly) NSTimeInterval position;

- (void)play;
- (void)pause;
- (void)forward;
- (void)rewind;

- (void)setMoviePosition:(NSTimeInterval)position;

@end
