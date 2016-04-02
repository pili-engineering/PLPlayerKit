//
//  PLPlayer.h
//  PLPlayerKit
//
//  Created on 15/10/15.
//  Copyright © 2015年 Pili Engineering. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "PLPlayerOption.h"

@class UIView;

/**
 PLPlayer 的播放状态
 
 @since v1.0.0
 */
typedef NS_ENUM(NSInteger, PLPlayerStatus) {
    
    /**
     PLPlayer 未知状态，只会作为 init 后的初始状态，开始播放之后任何情况下都不会再回到此状态。
     @since v1.0.0
     */
    PLPlayerStatusUnknow = 0,
    
    /**
     PLPlayer 正在准备播放所需组件，在调用 -play 方法时出现。
     
     @since v1.0.0
     */
    PLPlayerStatusPreparing,
    
    /**
     PLPlayer 播放组件准备完成，准备开始播放，在调用 -play 方法时出现。
     
     @since v1.0.0
     */
    PLPlayerStatusReady,
    
    /**
     @abstract PLPlayer 缓存数据为空状态。
     
     @discussion 特别需要注意的是当推流端停止推流之后，PLPlayer 将出现 caching 状态直到 timeout 后抛出 timeout 的 error 而不是出现 PLPlayerStatusStopped 状态，因此推流停止之后需要开发者实用业务服务器告知播放器停止播放。
     
     @since v1.0.0
     */
    PLPlayerStatusCaching,
    
    /**
     PLPlayer 正在播放状态。
     
     @since v1.0.0
     */
    PLPlayerStatusPlaying,
    
    /**
     PLPlayer 暂停状态。
     
     @since v1.0.0
     */
    PLPlayerStatusPaused,
    
    /**
     @abstract PLPlayer 停止状态
     @discussion 该状态仅会在回放时播放结束出现，RTMP 直播结束并不会出现此状态
     
     @since v1.0.0
     */
    PLPlayerStatusStopped,
    
    /**
     PLPlayer 错误状态，播放出现错误时会出现此状态。
     
     @since v1.0.0
     */
    PLPlayerStatusError
};

@class PLPlayer;

/**
 发送队列的代理协议。
 
 @since     v1.0.0
 */
@protocol PLPlayerDelegate <NSObject>

@optional

/**
 告知代理对象 PLPlayer 即将开始进入后台播放任务
 
 @param player 调用该代理方法的 PLPlayer 对象
 
 @since v1.0.0
 */
- (void)playerWillBeginBackgroundTask:(nonnull PLPlayer *)player;

/**
 告知代理对象 PLPlayer 即将结束后台播放状态任务
 
 @param player              调用该方法的 PLPlayer 对象
 
 @since v1.0.0
 */
- (void)playerWillEndBackgroundTask:(nonnull PLPlayer *)player;

/**
 告知代理对象播放器状态变更
 
 @param player 调用该方法的 PLPlayer 对象
 @param state  变更之后的 PLPlayer 状态
 
 @since v1.0.0
 */
- (void)player:(nonnull PLPlayer *)player statusDidChange:(PLPlayerStatus)state;

/**
 告知代理对象播放器因错误停止播放
 
 @param player 调用该方法的 PLPlayer 对象
 @param error  携带播放器停止播放错误信息的 NSError 对象
 
 @since v1.0.0
 */
- (void)player:(nonnull PLPlayer *)player stoppedWithError:(nullable NSError *)error;

@end

/**
 PLPlayer 是 PLPlayerKit 中负责播放控制的核心类
 
 @since v1.0.0
 */
@interface PLPlayer : NSObject

/**
 代理对象，用于告知播放器状态改变或其他行为，对象需实现 PLPlayerDelegate 协议
 
 @since v1.0.0
 */
@property (nonatomic, weak, nullable) id<PLPlayerDelegate>  delegate;

/**
 回调方法的调用队列
 
 @since v2.1.0
 */
@property (nonatomic, strong, nullable) dispatch_queue_t    delegateQueue;

/**
 支持音频后台播放的开关, 默认为 YES. 当 [AVAudioSession canPlayInBackground] 为 NO 时, 该值无效。
 
 @since v1.0.0
 */
@property (nonatomic, assign, getter=isBackgroundPlayEnable) BOOL backgroundPlayEnable;

/**
 @abstract      需要播放的 URL
 
 @discussion    目前支持 HLS (URL 以 http:// 开头) 与 rtmp (URL 以 rtmp:// 开头) 协议。
 
 @since v1.0.0
 */
@property (nonatomic, copy, nonnull, readonly) NSURL *  URL;

/**
 PLPlayer 的播放状态
 
 @since v1.0.0
 */
@property (nonatomic, assign, readonly) PLPlayerStatus   status;

/**
 PLPlayer 的 option 对象
 
 @since v2.1.0
 */
@property (nonnull, strong, readonly) PLPlayerOption *    option;

/**
 指示当前 PLPlayer 是否处于正在播放状态
 
 @since v1.0.0
 */
@property (nonatomic, assign, readonly, getter=isPlaying) BOOL  playing;

/**
 PLPlayer 的画面输出到该 UIView 对象
 
 @since v1.0.0
 */
@property (nonatomic, strong, nullable, readonly) UIView *  playerView;

/**
 使用 url 和 option 生成一个 PLPlayer 对象
 
 @param url    需要播放的 url ，目前支持 http (url 以 http:// 开头) 与 rtmp (url 以 rtmp:// 开头) 协议。
 @param option 播放器初始化选项，传入 nil 值将按照默认选项进行初始化
 
 @return 生成的PLPlayer 对象
 
 @since v2.1.0
 */
+ (nullable instancetype)playerWithURL:(nullable NSURL *)URL option:(nullable PLPlayerOption *)option;

/**
 使用 url 和 option 初始化一个 PLPlayer 对象
 
 @param url    需要播放的 url ，目前支持 http (url 以 http:// 开头) 与 rtmp (url 以 rtmp:// 开头) 协议。
 @param option 播放器初始化选项，传入 nil 值将按照默认选项进行初始化
 
 @return 初始化后的PLPlayer 对象
 
 @since v2.1.0
 */
- (nullable instancetype)initWithURL:(nullable NSURL *)URL option:(nullable PLPlayerOption *)option;

/**
 开始播放
 
 @since v1.0.0
 */
- (void)play;

/**
 当播放器处于暂停状态时调用该方法可以使播放器继续播放
 
 @since v1.0.0
 */
- (void)resume;

/**
 暂停播放器
 
 @since v1.0.0
 */
- (void)pause;

/**
 停止播放器
 
 @since v1.0.0
 */
- (void)stop;

@end

/**
 @discussion AVAudioSessionAvailabilityCheck 提供了与播放器相关的可用性检查的方法. 从 iOS 对 AVAudioSession 接口设计
 角度的考虑,PLPlayer 不会更改 AVAudioSession 的 category 值, 但是为了方便开发者, 我们提供一组类方法对sharedSession 做
 播放的可用性检查。你可以调用这些方法来做检查, 以确保最终音频播放的行为与你预期一致, 如果不一致, 请务必先阅读 AVAudioSession
 的相关文档再做设置。
 
 @since v1.0.0
 */
@interface AVAudioSession (AVAudioSessionAvailabilityCheck)

/**
 检查当前 AVAudioSession 的 category 配置是否可以播放音频。
 
 @return 当为 AVAudioSessionCategoryAmbient,AVAudioSessionCategorySoloAmbient, AVAudioSessionCategoryPlayback,
 AVAudioSessionCategoryPlayAndRecord中的一种时为 YES, 否则为 NO。
 
 @since v1.0.0
 */
+ (BOOL)isPlayable;

/**
 检查当前 AVAudioSession 的 category 配置是否可以后台播放。
 
 @return 当为 AVAudioSessionCategoryPlayback,AVAudioSessionCategoryPlayAndRecord 中的一种时为 YES, 否则为 NO。
 
 @since v1.0.0
 */
+ (BOOL)canPlayInBackground;

@end
