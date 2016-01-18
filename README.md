**重要说明**

因完成 PLPlayerKit 2.x 版本整体重构工作还需要较长时间，该 repo 的版本仍存在不少待解决的问题，会影响可用性。

在这样的情况下，为了满足直播播放器需求，我们整合 ijk player，调优参数额外提供一个稳定可用的版本以 zip 包形式提供，其中包含了 ijk 版本的 README 文档，example，`/deps` 目录下的两个 framework，下载链接如下（1月6日 release 版）:

http://7xpnwz.dl1.z0.glb.clouddn.com/PLIJKPlayerDemo-2016-01-06.zip

# PLPlayerKit

PLPlayerKit 是一个适用于 iOS 的音视频播放器 SDK，可高度定制化和二次开发，特色是支持 RTMP 和 HLS 直播流媒体播放。

功能特性

- [x] RTMP 直播流播放
- [x] HLS 播放
- [x] 高可定制
- [x] 音频后台播放
- [x] 无 ffmpeg 依赖

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
#import <PLPlayerKit/PLPlayer.h>
```

初始化

```Objective-C
// 初始化 Player
PLPlayer *player = [PLPlayer playerWithURL:self.url];

// 设定代理 (optional)
player.delegate = self;
```

开始／暂停操作

```Objective-C
// 准备播放器
__weak typeof(self) wself = self;
[self.player prepareToPlayWithCompletion:^(NSError *error) {
    if (!error) {
        __strong typeof(wself) strongSelf = wself;
        UIView *playerView = strongSelf.player.playerView;
        [strongSelf.view addSubview:playerView];
    }
}];
   
// 播放
[self.player play];
	
// 暂停
[self.player pause];
	
// 停止
[self.player stop];
```

播放器状态获取

```
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

```
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

## 已知 issues

- 当时间戳错乱时，player 当前处理有待优化，可能会因时间戳导致音频有卡顿现象。可以触发这一问题的行为有：
    - 推流端推流开始后，切换推流质量
    - Android 推流 SDK 切换前后至摄像头

## 版本历史

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