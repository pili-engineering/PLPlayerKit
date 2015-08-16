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
- (void)videoPlayerController:(PLVideoPlayerController *)controller playerStateDidChange:(PLPlayerState)state;
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
@property (nonatomic, assign) PLPlayerBackgroundMode   backgroundMode; // default as PLVideoPlayerBackgroundModeAutoPause

@property (nonatomic, assign, readonly) PLPlayerState playerState;
@property (nonatomic, readonly, getter=isPlaying) BOOL playing;
@property (nonatomic, assign, readonly) CGFloat audioVolume;
@property (nonatomic, assign, readonly) NSTimeInterval duration;
@property (nonatomic, assign, readonly) NSTimeInterval position;

/// 超时时长, 默认为 10s, 当超时发生时, player 便进入到 stopped 状态
@property (nonatomic, assign) NSTimeInterval    timeout;

/// 准备播放的方法主要完成流连接，解码器初始化等工作
- (void)prepareToPlayWithCompletion:(void (^)(BOOL success))handler;

/// 如果你没有主动的调用 prepareToPlayWithCompletion 方法, 直接调用 play 方法也是没有问题, play 方法内部会自行调用 prepareToPlayWithCompletion 方法来完成解码器初始化工作
- (void)play;

/// 暂停播放并不会彻底释放解码资源, 当调用 pause 后, 如果想恢复播放，再调用 play 便可
- (void)pause;

/// 停止播放会释放所有的解码资源, 当调用 stop 后, 如果想重新开始播放, 直接调用 play 便可, 但是因为要重新建立连接并初始化解码器, 所以比恢复 pause 的时间要长
- (void)stop;

- (void)forward;
- (void)rewind;

- (void)seekTo:(NSTimeInterval)position;

@end

@interface PLVideoPlayerController (Deprecated)

- (void)setMoviePosition:(NSTimeInterval)position DEPRECATED_ATTRIBUTE;

@end