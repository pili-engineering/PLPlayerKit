//
//  PLPlayer.h
//  PLPlayerKit
//
//  Created by 何昊宇 on 2017/5/15.
//  Copyright © 2017年 Aaron. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "PLPlayerOption.h"

@class UIView;
@class UIImageView;

/**
 @brief 音频采样格式
 
 @since 2.4.3
 */
typedef NS_ENUM(NSInteger, PLPlayerAVSampleFormat) {
    PLPlayerAV_SAMPLE_FMT_NONE = -1,
    PLPlayerAV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    PLPlayerAV_SAMPLE_FMT_S16,         ///< signed 16 bits
    PLPlayerAV_SAMPLE_FMT_S32,         ///< signed 32 bits
    PLPlayerAV_SAMPLE_FMT_FLT,         ///< float
    PLPlayerAV_SAMPLE_FMT_DBL,         ///< double
    
    PLPlayerAV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    PLPlayerAV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    PLPlayerAV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    PLPlayerAV_SAMPLE_FMT_FLTP,        ///< float, planar
    PLPlayerAV_SAMPLE_FMT_DBLP,        ///< double, planar
    
    PLPlayerAV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};

/**
 @brief 播放画面旋转模式
 
 @since v2.3.0
 */

typedef NS_ENUM(NSInteger, PLPlayerRotationsMode) {
    PLPlayerNoRotation, // 无旋转
    PLPlayerRotateLeft, // 向左旋
    PLPlayerRotateRight, // 向右旋
    PLPlayerFlipVertical, // 垂直翻转
    PLPlayerFlipHorizonal, // 水平翻转
    PLPlayerRotate180 // 旋转 180 度
};

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
     PLPlayer 播放组件准备完成，准备开始连接
     
     @warning 请勿在此状态时，调用 playWithURL 切换 URL 操作
     
     @since v3.2.1
     */
    PLPlayerStatusOpen,
    
    /**
     @abstract PLPlayer 缓存数据为空状态。
     
     @discussion 特别需要注意的是当推流端停止推流之后，PLPlayer 将出现 caching 状态直到 timeout 后抛出 timeout 的 error 而不是出现 PLPlayerStatusStopped 状态，因此在直播场景中，当流停止之后一般做法是使用 IM 服务告知播放器停止播放，以达到即时响应主播断流的目的。
     
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
    PLPlayerStatusError,
    
    /**
     *  PLPlayer 自动重连的状态
     */
    PLPlayerStateAutoReconnecting,
    
    /**
     *  PLPlayer 播放完成（该状态只针对点播有效）
     */
    PLPlayerStatusCompleted,
    
};

/**
 @brief 播放器音视频首帧数据类型
 
 @since v3.2.1
 */

typedef NS_ENUM(NSInteger, PLPlayerFirstRenderType) {
    PLPlayerFirstRenderTypeVideo = 0, // 视频
    PLPlayerFirstRenderTypeAudio // 音频
};

/**
 返回播放器 SDK 的版本信息的字符串。
 
 @since     v2.2.3
 */
extern NSString * _Nonnull playerVersion(void);

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
 
 @param player 调用该方法的 PLPlayer 对象
 
 @since v2.1.1
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

/**
 点播已缓冲区域
 
 @param player 调用该方法的 PLPlayer 对象
 @param timeRange  CMTime , 表示从0时开始至当前缓冲区域，单位秒。
 
 @warning 仅对点播有效
 
 @since v2.4.1
 */
- (void)player:(nonnull PLPlayer *)player loadedTimeRange:(CMTime)timeRange;

/**
 回调将要渲染的帧数据
 该功能只支持直播
 
 @param player 调用该方法的 PLPlayer 对象
 @param frame 将要渲染帧 数据,
 通过PLPlayerOptionKeyVideoOutputFormat【kPLPlayOutputFormatBGRA】，可修改输出格式
 默认输出格式 kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange
 
 @param pts 显示时间戳 单位ms
 @param sarNumerator
 @param sarDenominator
 其中sar 表示 storage aspect ratio
 视频流的显示比例 sarNumerator sarDenominator
 @discussion sarNumerator = 0 表示该参数无效
 
 @since v2.4.3
 */
- (void)player:(nonnull PLPlayer *)player willRenderFrame:(nullable CVPixelBufferRef)frame pts:(int64_t)pts sarNumerator:(int)sarNumerator sarDenominator:(int)sarDenominator;

/**
 回调音频数据
 
 @param player 调用该方法的 PLPlayer 对象
 @param audioBufferList 音频数据
 @param audioStreamDescription 音频格式信息
 @param pts 显示时间戳 是解码器进行显示帧时相对于SCR（系统参考）的时间戳。SCR可以理解为解码器应该开始从磁盘读取数据时的时间
 @param sampleFormat 采样位数 枚举：PLPlayerAVSampleFormat
 @return audioBufferList 音频数据
 
 @since v2.4.3
 */
- (nonnull AudioBufferList *)player:(nonnull PLPlayer *)player willAudioRenderBuffer:(nonnull AudioBufferList *)audioBufferList asbd:(AudioStreamBasicDescription)audioStreamDescription pts:(int64_t)pts sampleFormat:(PLPlayerAVSampleFormat)sampleFormat;

/**
 回调 SEI 数据
 
 @param player 调用该方法的 PLPlayer 对象
 @param SEIData SEI数据
 @since v3.0.1
 */
- (void)player:(nonnull PLPlayer *)player SEIData:(nullable NSData *)SEIData __deprecated_msg("Use player:SEIData:ts: instead");

/**
 回调 SEI 数据
 
 @param player 调用该方法的 PLPlayer 对象
 @param SEIData SEI数据
 @param ts 含有SEI数据的视频帧对应的时间戳
 @since v3.4.0
 */
- (void)player:(nonnull PLPlayer *)player SEIData:(nullable NSData *)SEIData ts:(int64_t)ts;

/**
 音视频渲染首帧回调通知
 
 @param player 调用该方法的 PLPlayer 对象
 @param firstRenderType 音视频首帧回调通知类型
 
 @since v3.2.1
 */
- (void)player:(nonnull PLPlayer *)player firstRender:(PLPlayerFirstRenderType)firstRenderType;

/**
 视频宽高数据回调通知

 @param player 调用该方法的 PLPlayer 对象
 @param width 视频流宽
 @param height 视频流高
 
 @since v3.3.0
 */
- (void)player:(nonnull PLPlayer *)player width:(int)width height:(int)height;

/**
 seekTo 完成的回调通知
 
 @param player 调用该方法的 PLPlayer 对象
 
 @since v3.3.0
 */
- (void)player:(nonnull PLPlayer *)player seekToCompleted:(BOOL)isCompleted;

@end

/**
 getScreenShotWithCompletionHandler 截图操作为异步，
 完成后将通过 ScreenShotWithCompletionHandler 类型的 block 回调返回 UIImage 类型图片数据。
 
 @since v2.2.3
 */

typedef void (^ScreenShotWithCompletionHandler)(UIImage * _Nullable image);


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
 支持音频后台播放的开关, 默认为 YES. 请确认 [AVAudioSession canPlayInBackground] 为 YES。
 
 @warning 当设置允许后台播放时，建议 iOS 9 及以下系统，在 AppDelegate.m 下的即将进入后台： - (void)applicationWillResignActive:(UIApplication *)application 里，设置 enableRender 为 NO 以取消画面渲染；已经进入前台：- (void)applicationDidBecomeActive:(UIApplication *)application 里，设置 enableRender 为 YES 以恢复画面渲染
 
 @since v1.0.0
 */
@property (nonatomic, assign, getter=isBackgroundPlayEnable) BOOL backgroundPlayEnable;

/**
 @abstract      需要播放的 URL
 
 @discussion    目前支持 http(s) (url 以 http:// https:// 开头) 与 rtmp (URL 以 rtmp:// 开头) 协议。
 
 @since v1.0.0
 */
@property (nonatomic, copy, nonnull, readonly) NSURL *URL;

/**
 PLPlayer 的播放状态
 
 @since v1.0.0
 */
@property (nonatomic, assign, readonly) PLPlayerStatus   status;

/**
 PLPlayer 的 option 对象
 
 @since v2.1.0
 */
@property (nonnull, strong, readonly) PLPlayerOption *option;

/**
 指示当前 PLPlayer 是否处于正在播放状态
 
 @since v1.0.0
 */
@property (nonatomic, assign, readonly, getter=isPlaying) BOOL  playing;

/**
 PLPlayer 的画面输出到该 UIView 对象
 
 @since v1.0.0
 */
@property (nonatomic, strong, nullable, readonly) UIView *playerView;

/**
 PLPlayer 的启动图
 
 @discussion 播放开始前显示的图片。
 
 @since v2.4.0
 */
@property (nonatomic, strong, nullable) UIImageView *launchView;

/**
 是否需要静音 PLPlayer，默认值为NO
 
 @since v2.1.2
 */
@property (nonatomic, assign, getter=isMute) BOOL mute;

/**
 PLPlayer 的当前播放时间，仅回放状态下有效，只播放状态下将返回 CMTime(0,30)
 
 @since v2.1.2
 */
@property (nonatomic, assign, readonly) CMTime  currentTime;

/**
 PLPlayer 的总播放时间，仅回放状态下有效，只播放状态下将返回 CMTime(0,30)
 
 @since v2.1.2
 */
@property (nonatomic, assign, readonly) CMTime  totalDuration;

/**
 是否开启重连，默认为 NO
 
 @warning 该属性仅对 rtmp/flv 直播与 ffmpeg 点播有效
 
 @since v2.2.2
 */
@property (nonatomic, assign, getter = isAutoReconnectEnable) BOOL autoReconnectEnable;

/**
 设置画面旋转模式
 
 @warning 该属性仅对 rtmp/flv 直播与 ffmpeg 点播有效
 
 @since v2.3.0
 */
@property (nonatomic, assign) PLPlayerRotationsMode rotationMode;

/**
 是否渲染画面，默认为 YES
 
 @warning 该属性仅对 rtmp/flv 直播与 ffmpeg 点播有效
 
 @since v2.3.0
 */
@property (nonatomic, assign) BOOL enableRender;

/**
 设置 http header referer 值
 
 @since v2.4.1
 */
@property (nonatomic, strong) NSString * _Nonnull referer;

#pragma mark -- play info

/**
 meta data
 
 @since v2.4.0
 */
@property (nonatomic, strong, readonly) NSDictionary * _Nullable metadata;

/**
 连接时间
 从触发播放到建立连接的耗时
 
 @since v2.4.3
 */

@property (nonatomic, assign, readonly) NSTimeInterval connectTime;

/**
 首开时间
 从触发播放到第一帧视频渲染的耗时
 
 @since v2.4.3
 */

@property (nonatomic, assign, readonly) NSTimeInterval firstVideoTime;

/**
 视频流的宽
 
 @warning 该属性仅对 rtmp/flv 直播与 ffmpeg 点播有效
 
 @since v2.3.0
 */
@property (nonatomic, assign, readonly) int width;

/**
 视频流的高
 
 @warning 该属性仅对 rtmp/flv 直播与 ffmpeg 点播有效
 
 @since v2.3.0
 */
@property (nonatomic, assign, readonly) int height;

/**
 视频流的帧率
 
 @warning 该属性仅 rtmp/flv 直播有效。
 
 @since v2.3.0
 */
@property (nonatomic, assign, readonly) int videoFPS;

/**
 播放渲染帧率
 
 @warning 该属性仅对 rtmp/flv 直播与 ffmpeg 点播有效
 
 @since v2.3.0
 */
@property (nonatomic, assign, readonly) int renderFPS;

/**
 视频流的码率，单位 kb/s
 
 @warning 该属性仅 rtmp/flv 直播有效。
 
 @since v2.3.0
 */
@property (nonatomic, assign, readonly) double videoBitrate;


/**
 音频流的帧率
 
 @warning 该属性仅 rtmp/flv 直播有效。
 
 @since v3.4.6
 */
@property (nonatomic, assign, readonly) int audioFPS;



/**
 视频流的码率，单位 kb/s
 
 @warning 该属性仅 rtmp/flv 直播有效。
 
 @since v3.4.6
 */
@property (nonatomic, assign, readonly) double audioBitrate;

/**
 下载速率，单位 kb/s
 
 @warning 该属性仅对 rtmp/flv 直播与 ffmpeg 点播有效
 
 @since v2.3.0
 */
@property (nonatomic, assign, readonly) double downSpeed;

/**
 私有DRM
 
 @warning 该属性仅对 HLS 点播有效
 
 @since v3.0.0
 */
@property (nonatomic, strong) NSString * _Nullable DRMKey;

/**
 用户秘钥
 
 @warning 该属性仅对使用 七牛转码 加密的 MP4 点播有效
 
 @since v3.4.1
 */
@property (nonatomic, strong) NSString * _Nullable DrmCompanyKey;

/**
 变速播放，范围是 0.2-32，默认是 1.0
 
 @warning 该属性仅对点播有效
 
 @since v3.0.0
 */
@property (nonatomic, assign) double playSpeed;

/**
  rtmp 流中的视频时间戳信息
 
 @warning 该属性仅对 rtmp 有效
 
 @since v3.1.0
 */
@property (nonatomic, assign, readonly) CMTime rtmpVideoTimeStamp;

/**
 rtmp 流中的音频时间戳信息
 
 @warning 该属性仅对 rtmp 有效
 
 @since v3.1.0
 */
@property (nonatomic, assign, readonly) CMTime rtmpAudioTimeStamp;

/**
 视频旋转角度
 
 @warning 该属性属于视频本身旋转角度
 
 @since v3.1.0
 */
@property (nonatomic, assign, readonly) int rotate __deprecated_msg("Method deprecated in v3.2.1. Adjust automatically");

/**
 视频剪裁播放，配置参数：(x, y, width, height)，尺寸需是 4 的倍数，默认全视频画面
 
 @warning 该属性只播放和渲染指定位置的画面，其中 x、y、width、height 值均取整使用，若播放新链接需裁剪，则每次新链接 play 前重新设置；若需重置为全视频画面，传 CGRectZero 即可
 
 @since v3.2.1
 */
@property (nonatomic, assign) CGRect videoClipFrame;

/**
 是否循环播放，默认为 NO
 
 @warning 该属性仅对点播有效
 
 @since v3.2.1
 */
@property (nonatomic, assign) BOOL loopPlay;

/**
 提前使用 DNS 解析 URL 中的域名。
 
 @discussion 在初始化后，播放前调用该方法，预解析播放地址的域名。
 
 @since v3.3.1
 */
- (void)preDNSHost:(nullable NSURL *)URL;


/**
 提前设置 mp4 解析时间。
 
 @since v3.3.1
 */
- (void)mp4PreLoadTime:(CMTime)loadTime;


/**
 提前设置点播从某记忆点开始播放。
 
 @since v3.3.1
 */
- (void)preStartPosTime:(CMTime)startTime;


/**
 使用 url 和 option 生成一个 PLPlayer 对象, 直播使用此接口
 
 @param URL    需要播放的 url ，目前支持 http(s) (url 以 http:// https:// 开头) 与 rtmp (url 以 rtmp:// 开头) 协议。
 @param option 播放器初始化选项，传入 nil 值将按照默认选项进行初始化
 
 @return 生成的PLPlayer 对象
 
 @since v2.2.3
 */
+ (nullable instancetype)playerLiveWithURL:(nullable NSURL *)URL option:(nullable PLPlayerOption *)option;

/**
 使用 url 和 option 生成一个 PLPlayer 对象，点播使用此接口
 
 @param URL    需要播放的 url ，目前支持 http(s) (url 以 http:// https:// 开头) 与 rtmp (url 以 rtmp:// 开头) 协议。
 @param option 播放器初始化选项，传入 nil 值将按照默认选项进行初始化
 
 @return 生成的PLPlayer 对象
 
 @since v2.2.3
 */
+ (nullable instancetype)playerWithURL:(nullable NSURL *)URL option:(nullable PLPlayerOption *)option;

/**
 使用 url 和 option 初始化一个 PLPlayer 对象
 
 @param URL    需要播放的 url ，目前支持 http(s) (url 以 http:// https:// 开头) 与 rtmp (url 以 rtmp:// 开头) 协议。
 @param option 播放器初始化选项，传入 nil 值将按照默认选项进行初始化
 
 @return 初始化后的 PLPlayer 对象
 
 @warning 不支持使用 init, new 初始化 PLPlayer 对象。
 
 @since v2.1.0
 */
- (nullable instancetype)initWithURL:(nullable NSURL *)URL option:(nullable PLPlayerOption *)option;

/**
 URL 预加载
 
 @discussion 在播放前调用该方法，预加载播放 URL 链接。
 
 @warning 请勿与 playWithURL 切换新的 URL 合用，容易出现切换失败，播放流错误等问题
 
 @return 是否成功打开
 
 @since v3.2.1
 */
- (BOOL)openPlayerWithURL:(nullable NSURL *)URL;

/**
 开始播放
 @return 是否成功播放
 
 @since v3.2.1
 */
- (BOOL)play;

/**
 开始播放新的 url
 
 @param URL 需要播放的 url ，目前支持 http(s) (url 以 http:// https:// 开头) 与 rtmp (url 以 rtmp:// 开头) 协议。
 
 @return 是否成功播放

 @since v3.2.1
 */
- (BOOL)playWithURL:(nullable NSURL *)URL __deprecated;

/**
 开始播放新的 url
 
 @param URL 需要播放的 url ，目前支持 http(s) (url 以 http:// https:// 开头) 与 rtmp (url 以 rtmp:// 开头) 协议。
 @param sameSource 是否是同种格式播放，同格式切换打开更快
 
 @return 是否成功播放

 @warning  当sameSource 为 YES 时，视频格式与切换前视频格式不同时，会导致视频打开失败
 
 @since v3.2.1
 */
- (BOOL)playWithURL:(nullable NSURL *)URL sameSource:(BOOL)sameSource;

/**
 当播放器处于暂停状态时调用该方法可以使播放器继续播放
 
 @since v1.0.0
 */
- (void)resume;

/**
 当播放器处于 playing 或 caching 状态时调用该方法可以暂停播放器
 
 @since v1.0.0
 */
- (void)pause;

/**
 停止播放器
 
 @since v1.0.0
 */
- (void)stop;

/**
 快速定位到指定播放时间点，该方法仅在回放时起作用，直播场景下该方法直接返回
 
 @param time 需要
 
 @since v2.1.2
 */
- (void)seekTo:(CMTime)time;

/**
 *  设置音量，范围是0-3.0，默认是1.0
 *
 *  @param volume 音量
 *
 *  @since v2.2.3
 */
- (void)setVolume:(float)volume;

/**
 *  获取音量
 *
 *  @since v2.2.3
 *
 *  @return 音量
 */
- (float)getVolume;

/**
 *  截图
 *  @param handle 类型 ScreenShotWithCompletionHandler block 。
 *
 *  @discussion 截图操作为异步，完成后将通过 handle 回调返回 UIImage 类型图片数据。
 *              该功能只支持软解
 *
 *  @since v2.2.3
 *
 */
- (void)getScreenShotWithCompletionHandler:(nullable ScreenShotWithCompletionHandler)handle;

/**
 *  获取缓冲的文件字节数，已缓冲还未播放的值
 *
 *  @since v3.3.0
 *
 *  @return 文件字节数 long long 类型
 *
 */
- (long long)getHttpBufferSize;

/**
 *  是否缓存下载
 *  @param bufferingEnabled 是否允许，默认为 YES，允许缓存下载
 *
 *  @discussion 当 bufferingEnabled 为 YES 时，文件正常下载。若下载中，修改 bufferingEnabled 为 NO，则下载暂停
 *
 *  @since v3.3.0
 *
 */
- (void)setBufferingEnabled:(BOOL)bufferingEnabled;

/**
 *  获取是否允许缓存下载
 *
 *  @since v3.3.0
 *
 *  @return 是否允许缓存下载
 */
- (BOOL)getBufferingEnabled;

/**
 *  添加预缓存下载
 *
 *  @since v3.4.6
 *
 */
- (void)addCacheUrl:(NSString *_Nullable)url;


/**
 *  移除预缓存下载
 *
 *  @since v3.4.6
 *
 */
- (void)deleteCacheUrl:(NSString *_Nullable)url;


/**
 *  addIOCache
 *
 *  @since v3.4.6
 *
 */
- (void)addIOCache:(NSString *_Nullable)url;


/**
 *  deleteIOCache
 *
 *  @since v3.4.6
 *
 */
- (void)deleteIOCache:(NSString *_Nullable)url;

/**
 *  Set IOCache size
 *
 *  @since v3.4.6
 *
 */
- (void)setIOCacheSize:(NSInteger)size;


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
