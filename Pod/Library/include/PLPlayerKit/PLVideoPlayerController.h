//
//  PLVideoPlayerController.h
//  PLPlayerKit
//
//  Created on 15/5/6.
//  Copyright (c) 2015年 Pili Engineering. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PLPlayerTypeDefines.h"

@class PLVideoPlayerController;
@protocol PLVideoPlayerControllerDelegate <NSObject>

@optional
- (void)videoPlayerController:(PLVideoPlayerController *)controller playerStateDidChange:(PLVideoPlayerState)state;
- (void)videoPlayerControllerDecoderHasBeenReady:(PLVideoPlayerController *)controller;
- (void)videoPlayerController:(PLVideoPlayerController *)playerController failureWithError:(NSError *)error;
- (void)videoPlayerController:(PLVideoPlayerController *)playerController positionDidChange:(NSTimeInterval)position;

@end

@interface PLVideoPlayerController : NSObject

+ (instancetype)videoPlayerControllerWithContentURL:(NSURL *)url
                                         parameters:(NSDictionary *)parameters;

@property (nonatomic, weak) id<PLVideoPlayerControllerDelegate> delegate;
@property (nonatomic, readonly, strong) UIView    *playerView;
@property (nonatomic, assign) BOOL userInteractionEnable;   // default as YES
@property (nonatomic, assign, getter=isMuted) BOOL  muted;  // default as NO
@property (nonatomic, assign) PLVideoPlayerBackgroundMode   backgroundMode; // default as PLVideoPlayerBackgroundModeAutoPause

@property (nonatomic, assign, readonly) PLVideoPlayerState playerState;
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
