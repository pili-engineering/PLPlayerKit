//
//  KxMovieController.h
//  kxmovie
//
//  Created by 0day on 15/5/6.
//
//

#import <Foundation/Foundation.h>
#import "KxTypeDefines.h"

@class KxMovieDecoder;
@class KxMovieController;
@protocol KxMovieControllerDelegate <NSObject>

@optional
- (void)movieController:(KxMovieController *)controller playerStateDidChange:(KxPlayerState)status;
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

@property (nonatomic, assign, readonly) KxPlayerState playerState;
@property (nonatomic, readonly, getter=isPlaying) BOOL playing;
@property (nonatomic, assign, readonly) CGFloat audioVolume;
@property (nonatomic, assign, readonly) NSTimeInterval duration;
@property (nonatomic, assign, readonly) NSTimeInterval position;

/// 超时时长，默认为 3s
@property (nonatomic, assign) NSTimeInterval    timeout;

- (void)prepareToPlayWithCompletion:(void (^)(BOOL success))handler;
- (void)play;
- (void)pause;
- (void)stop;
- (void)forward;
- (void)rewind;

- (void)seekTo:(NSTimeInterval)position;

@end
