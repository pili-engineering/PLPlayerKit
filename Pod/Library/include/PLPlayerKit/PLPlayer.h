//
//  PLPlayer.h
//  PLPlayerKit
//
//  Created on 15/10/15.
//  Copyright © 2015年 Pili Engineering. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@class UIView;
typedef NS_ENUM(NSInteger, PLPlayerStatus) {
    PLPlayerStatusUnknow = 0,
    PLPlayerStatusPreparing,
    PLPlayerStatusReady,
    PLPlayerStatusCaching,
    PLPlayerStatusPlaying,
    PLPlayerStatusPaused,
    PLPlayerStatusStopped,
    PLPlayerStatusError
};

@class PLPlayer;
@protocol PLPlayerDelegate <NSObject>

@optional

/// 后台任务启动及关闭的回调
- (void)playerWillBeginBackgroundTask:(nonnull PLPlayer *)player;
- (void)player:(nonnull PLPlayer *)player willEndBackgroundTask:(BOOL)isExpirationOccured;

/// status 变更回调
- (void)player:(nonnull PLPlayer *)player statusDidChange:(PLPlayerStatus)state;
- (void)player:(nonnull PLPlayer *)player stoppedWithError:(nullable NSError *)error;

@end

@interface PLPlayer : NSObject

@property (nonatomic, strong, nonnull) NSURL    *url;
@property (nonatomic, weak, nullable) id<PLPlayerDelegate>    delegate;
/// 接收/发送数据包超时时间间隔, 默认为 10s.
/// 注意: 当前, 该属性在 - (void)prepareToPlayWithCompletion: 的 completion block 中执行才会有效
@property (nonatomic, assign) NSTimeInterval    timeoutIntervalForMediaPackets;

@property (nonatomic, strong, nullable) NSError   *error;
@property (nonatomic, assign) PLPlayerStatus   status;
@property (nonatomic, assign, getter=isPlaying) BOOL  playing;

/*!
 * @description 支持音频后台播放的开关, 默认为 YES. 当 [AVAudioSession canPlayInBackground] 为 NO 时, 该值无效。
 */
@property (nonatomic, assign, getter=isBackgroundPlayEnable) BOOL backgroundPlayEnable;

// render
/// 注意: playerView 必须在调用完 - (void)prepareToPlayWithCompletion: 后才能访问到正常的值
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

/*!
 * AVAudioSessionAvailabilityCheck 提供了与播放器相关的可用性检查的方法. 从 iOS 对 AVAudioSession 接口
 * 设计角度的考虑, PLPlayer 不会更改 AVAudioSession 的 category 值, 但是为了方便开发者, 我们提供一组类方法对
 * sharedSession 做播放的可用性检查.
 *
 * 你可以调用这些方法来做检查, 以确保最终音频播放的行为与你预期一致, 如果不一致, 请务必先阅读 AVAudioSession 的
 * 相关文档再做设置
 */
@interface AVAudioSession (AVAudioSessionAvailabilityCheck)

/*!
 * @description 检查当前 AVAudioSession 的 category 配置是否可以播放音频. 当为 AVAudioSessionCategoryAmbient,
 * AVAudioSessionCategorySoloAmbient, AVAudioSessionCategoryPlayback, AVAudioSessionCategoryPlayAndRecord 
 * 中的一种时为 YES, 否则为 NO.
 */
+ (BOOL)isPlayable;

/*!
 * @description 检查当前 AVAudioSession 的 category 配置是否可以后台播放. 当为 AVAudioSessionCategoryPlayback,
 * AVAudioSessionCategoryPlayAndRecord 中的一种时为 YES, 否则为 NO.
 */
+ (BOOL)canPlayInBackground;

@end
