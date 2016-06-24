# PLPlayerKit

PLPlayerKit 是一个适用于 iOS 的音视频播放器 SDK，可高度定制化和二次开发，特色是支持 RTMP, HTTP-FLV 和 HLS 直播流媒体播放。

功能特性

- [x] RTMP 直播流播放
- [x] HTTP-FLV 直播流播放
- [x] HLS 播放
- [x] 高可定制
- [x] 音频播放
- [x] RTMP 直播首屏秒开支持
- [x] RTMP 直播累积延迟消除技术


## 内容摘要

- [快速开始](#1-快速开始)
	- [配置工程](#配置工程)
	- [示例代码](#示例代码)
- [关于 2.0 版本](#关于2.0版本)
- [版本历史](#版本历史)

## 快速开始

### 配置工程

- 配置你的 Podfile 文件，添加如下配置信息

```
pod 'PLPlayerKit'
```

- 安装 CocoaPods 依赖

```
pod update
```
or
```
pod install
```

- Done! 运行你工程的 workspace

### 示例代码

在需要的地方添加

```Objective-C
#import <PLPlayerKit/PLPlayerKit.h>
```

初始化 PLPlayerOption

```Objective-C
// 初始化 PLPlayerOption 对象
PLPlayerOption *option = [PLPlayerOption defaultOption];

// 更改需要修改的 option 属性键所对应的值
[option setOptionValue:@15 forKey:PLPlayerOptionKeyTimeoutIntervalForMediaPackets];
[option setOptionValue:@1000 forKey:PLPlayerOptionKeyMaxL1BufferDuration];
[option setOptionValue:@1000 forKey:PLPlayerOptionKeyMaxL2BufferDuration];
[option setOptionValue:@(YES) forKey:PLPlayerOptionKeyVideoToolbox];
[option setOptionValue:@(kPLLogInfo) forKey:PLPlayerOptionKeyLogLevel];
[option setOptionValue:[QNDnsManager new] forKey:PLPlayerOptionKeyDNSManager];

```

初始化 PLPlayer

```Objective-C
// 初始化 PLPlayer
self.player = [PLPlayer playerWithURL:self.URL option:option];

// 设定代理 (optional)
self.player.delegate = self;
```

获取播放器的视频输出的 UIView 对象并添加为到当前 UIView 对象的 Subview
```Objective-C
//获取视频输出视图并添加为到当前 UIView 对象的 Subview
[self.view addSubview:player.playerView];
```

开始／暂停操作

```Objective-C

// 播放
[self.player play];

// 停止
[self.player stop];

// 暂停
[self.player pause];

// 继续播放
[self.player resume];
```

播放器状态获取

```Objective-C
// 实现 <PLPlayerDelegate> 来控制流状态的变更
- (void)player:(nonnull PLPlayer *)player statusDidChange:(PLPlayerStatus)state {
	// 这里会返回流的各种状态，你可以根据状态做 UI 定制及各类其他业务操作
	// 除了 Error 状态，其他状态都会回调这个方法
}

- (void)player:(nonnull PLPlayer *)player stoppedWithError:(nullable NSError *)error {
	// 当发生错误时，会回调这个方法
}
```

## 音频部分的特别说明

因为 iOS 的音频资源被设计为单例资源，所以如果在 player 中做的任何修改，对外都可能造成影响，并且带来不能预估的各种问题。

为了应对这一情况，PLPlayerKit 采取的方式是检查是否可以播放及是否可以进入后台，而在内部不做任何设置。具体是通过扩展 `AVAudioSession` 来做到的，提供了两个方法，如下：

```Objective-C
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
```

分辨可以检查是否可以播放以及当前 category 的设置是否可以后台播放。

## 版本历史
- 2.2.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.2.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.2.1.md))
- 功能
  - 支持 SDK 日志级别设置
  - 新增 HappyDNS 支持
- 缺陷
  - 修复回看状态不准确问题
  - 修复跳转第三方应用，出现内存增加。
  - 修复播放卡住 caching 状态。
- 2.2.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.2.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.2.0.md))
- 功能
	- 新增硬解功能
	- 新增 http-flv 支持
	- 新增 iOS9 下的纯 IPV6 环境支持
- 缺陷
	- 修复快速进入退出黑屏
- 优化
	- 追帧策略优化
	- 退出后台停止视频解码
- 2.1.3 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.1.3.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.1.3.md))
	- 增加设置一级缓存和二级缓存的选项，便于控制卡顿率
	- 修复播放 OBS 及 FFmpeg 推的流黑屏的问题
	- 修复播放结束后无法重播的问题
	- 修复播放过程中内存暴增的问题
	- 拆分 pili-librtmp 为公共依赖，解决模拟器环境下与 PLStreamingKit 冲突的问题
- 2.1.2 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.1.2.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.1.2.md))
	- 增加确切的错误枚举，方便定位错误类型
	- 增加 mute, currentTime, totalDuration, seekTo 等接口
	- 修复首屏开启以及播放过程中出现缓存后网络恢复是可能出现的 UI 卡顿问题
	- 修复 contentMode 偶尔设置无效的问题
	- 修复重新设置播放 url 播放的问题
	- 修复快速 -stop 以及 -play 出现的内存泄露问题
- 2.1.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.1.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.1.1.md))
	- 首屏开启速度优化，在网络状况良好的情况下能实现秒开效果
	- 弱网情况下的累积延迟问题优化，较好控制累积延迟在数秒以内
	- 解决了上一版遇到的无法设置 playerView.contentMode 以及 playerOption 的问题
	- 解决了不标准流可能出现的音频断续，播放器内存异常增长问题
	- 后台播放体验优化，修复了后台播放被其他音频打断后出现的一系列问题
	- 解决了应用切换时出现的 UI 卡死问题
- 2.1.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.1.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.1.0.md))
	- 此次更新为重大版本升级，更改了大量 API 并重构了包括解码渲染在内的多项内容，建议所有用户进行升级，并且根据[快速开始](#快速开始)使用新版 API 对工程重新进行配置。
	- 更改了播放器的音频解码和渲染方式
	- 更改了播放器的时钟同步机制
	- 重构了内部逻辑，使播放器更稳定
	- 重构了播放器 API ，使播放器的使用更加简单明了，去除了使用起来不方便的部分 API
	- 解决了播放过程中可能出现声音消失的问题
	- 解决了退后台返回后音视频无法正常同步的问题
	- 修改播放器音视频同步机制
	- 解决持续播放过程中出现部分内存没有正确释放的问题
	- 解决了 iOS 版本小于 8.0 时 Demo 出现的crash问题
- 2.0.4 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.0.4.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.0.4.md))
  - 解决 RTMP 播放时可能黑屏的问题
- 2.0.3 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.0.3.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.0.3.md))
  - 解决 RTMP 播放没有声音
  - 解决 RTMP 无法播放导致内存急增最终 App crash
  - 解决 RTMP 无法播放画面只有声音
  - 解决播放 RTMP 时相关的 crash 问题
- 2.0.2 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.0.2.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.0.2.md))
  - 添加 RTMP Cache 机制
  - 添加数据超时属性
  - 修复 RTMP 播放内存 leak
  - 修复 RTMP 播放音频错误问题
  - 修复 RTMP 播放主线程卡死问题
  - 优化架构，减少内存和 cpu 占用
- 2.0.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.0.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.0.1.md))
  - 修复 `contentMode` 设置无效的问题
  - 修复 rtmp 无法播放或播放超时时无 error 抛出的问题
  - 修复 rtmp 播放失败时触发的 cpu 飙升问题
  - 修复 stop 可能触发的 crash 问题
  - 更新 demo 确保在 iOS 9.1 下运行正常
- 2.0.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.0.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.0.0.md))
  - 添加全新的 `PLPlayer`，弃用 `PLVideoPlayerController` 和 `PLAudioPlayerController`
  - 播放 RTMP 音视频流时，进入后台后声音继续播放，不会断开，返回前台追帧显示最新视频帧
  - 针对 RTMP 直播彻底优化，首屏秒开，最小化缓存
  - 完全无 ffmpeg 依赖，包体积再次缩小
  - 优化资源占用，比 1.x 版本内存占用减少 50% 以上
- 1.2.22 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.22.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.22.md))
  - 修复因收到内存警告而引起的崩溃问题
  - 修复停止播放时，可能进入错误 play state 的问题
- 1.2.21 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.21.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.21.md))
  - 修复 `PLVideoParameterFrameViewContentMode` 与 `PLVideoParameterDisableDeinterlacing` 设置无效的问题
- 1.2.20 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.20.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.20.md))
  - 修复 `seekTo:` 不准确的问题
  - 添加 `PLPlayerStateSeeking` 类型
- 1.2.19 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.19.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.19.md))
  - 修复播放无返回状态的问题（针对无直播的流、hls 回放）
  - 修复 hls 回放结束时无 stopped 回调的问题
  - 修复 hls 回放开始的 duration 不为 0 的问题
- 1.2.18 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.18.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.18.md))
  - 修复在 prepare 状态前释放 player 导致的音频仍然会播放的问题
  - 修复 player 状态返回的类型不正确的问题
  - 优化推出时资源释放
- 1.2.17 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.17.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.17.md))
  - 修复超时时导致的崩溃的问题
- 1.2.16 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.17.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.16.md))
  - 添加了音频播放器后台播放的支持
  - 添加了音频播放器后台播放任务开始和结束的回调
  - 添加了音视频播放器超时时长的设定
  - 添加了音视频播放器准备的方法
  - 添加了音视频完全停止播放器的方法
  - 修复播放器不可释放的问题
- 1.2.15 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.15.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.15.md))
  - 修复 AudioPlayer 无法播放带有视频流的 RTMP 流的问题
- 1.2.14 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.14.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.14.md))
  - 添加 AudioManager
- 1.2.13 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.13.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.13.md))
  - 添加纯音频播放控件
  - 更新参数字段及类型，确保通用类型可以在音频及视频播放器使用
  - 更新类型名称，增加易读性，减少歧义
- 1.2.12 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.12.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.12.md))
	- 更改 repo 地址
- 1.2.11 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.11.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.11.md))
	- 添加对应用状态的判断，减少因进入后台通知延时未能及时暂停播放导致的 crash
- 1.2.10 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.10.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.10.md))
	- 添加音频外设更改时的通知
	- 添加音量变更时的通知
	- 添加打进电话等其他事件导致音频中断的通知
- 1.2.9 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.9.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.9.md))
	- 修复进入后台后崩溃的问题
	- 更新 example 中 player 代码，支持横竖屏旋转操作
- 1.2.8 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.8.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.8.md))
	- 添加播放进度回调方法
	- 修复 seekTo 后流状态不正确的问题
- 1.2.7 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.7.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.7.md))
	- 添加播放器状态属性
	- 添加解码器初始化完成后回调
	- 添加播放器状态回调
	- 添加初始化后自动播放参数
- 1.2.6 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.6.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.6.md))
	- 添加设置播放位置的操作
	- 添加了快进、快退的操作
	- 添加总播放时长的属性
	- 添加获取音量的属性
	- 添加获取当前播放位置的属性
	- 添加静音操作
- 1.2.5 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.5.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.5.md))
	- 修复与部分其他库头文件冲突的问题
- 1.2.4 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.4.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.4.md))
	- 添加了 ```PLMovieParameterFrameViewContentMode``` 参数
	- 修复与部分其他库头文件冲突的问题
	- 修复 Player contentMode 无法更改的问题
- 1.2.3 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.3.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.3.md))
	- 修复初始化占用主线程导致卡顿的问题
	- 修复错误回调无效的问题
- 1.2.2 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.2.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.2.md))
	- 修复 lib 未更新导致的 crash
- 1.2.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.1.md))
	- 添加 failue 情况下的回调，返回 NSError 对象
	- 移除 PLVideoPlayerViewController，请直接使用 PLVideoPlayerController 进行定制
- 1.2.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.0.md))
	- 极大缩小 lib 大小
	- 增加可定制的播放控件 PLVideoPlayerController
- 1.1.2
	- 拆分 Flat lib
	- 添加了 x86_64 支持，便于在 iPhone 6 Plus 模拟器下调试使用
- 1.1.1
	- 对库引用做了些修改
- 1.1.0
	- 发布 CocoaPods 版本
