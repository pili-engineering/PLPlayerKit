//
//  PLPlayer.h
//  PLPlayerKit
//
//  Created on 15/10/15.
//  Copyright © 2015年 Pili Engineering. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>

@class UIView;
typedef NS_ENUM(NSInteger, PLPlayerStatus) {
    PLPlayerStatusUnknow = 0,
    PLPlayerStatusPreparing,
    PLPlayerStatusReady,
    PLPlayerStatusPlaying,
    PLPlayerStatusPaused,
    PLPlayerStatusStopped,
    PLPlayerStatusError
};

@class PLPlayer;
@protocol PLPlayerDelegate <NSObject>

- (void)player:(nonnull PLPlayer *)player statusDidChange:(PLPlayerStatus)state;
- (void)player:(nonnull PLPlayer *)player stoppedWithError:(nullable NSError *)error;

@end

@interface PLPlayer : NSObject

@property (nonatomic, strong, nonnull) NSURL    *url;
@property (nonatomic, assign, nullable) id<PLPlayerDelegate>    delegate;

@property (nonatomic, strong, nullable) NSError   *error;
@property (nonatomic, assign) PLPlayerStatus   status;
@property (nonatomic, assign, getter=isPlaying) BOOL  playing;

// render
/// 注意：playerView 必须在调用完 - (void)prepareToPlayWithCompletion: 后才能访问到正常的值
@property (nonatomic, strong, nullable) UIView    *playerView;

+ (nullable instancetype)playerWithURL:(nullable NSURL *)url;
- (nullable instancetype)initWithURL:(nullable NSURL *)url;

/// completion 回调会在主线程进行调用，所以可以放心在回调中对 UI 做操作
- (void)prepareToPlayWithCompletion:(nullable void (^)(NSError * _Nullable error))completion;
- (void)play;
- (void)resume;
- (void)pause;
- (void)stop;

@end
