# PLPlayerKit

PLPlayerKit 是一个适用于 iOS 的音视频播放器 SDK，可高度定制化和二次开发，特色是支持 RTMP 和 HLS 直播流媒体播放，并且支持常见音视频文件例如 MP4/M4A 的播放。

功能特性

- [x] RTMP 直播流播放
- [x] HLS 播放
- [x] 高可定制
- [x] 渲染逐行扫描支持
- [x] 缓存时长可定制
- [x] 支持本地及线上音视频文件直接播放
- [x] 音频后台播放

## 内容摘要

- [1 快速开始](#1-快速开始)
	- [1.1 配置工程](#1.1-配置工程)
	- [1.2 示例代码](#1.2-示例代码)
- [2 第三方库](#2-第三方库)
- [3 系统要求](#3-系统要求)
- [4 版本历史](#4-版本历史)

## 1 快速开始

### 1.1 配置工程

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

### 1.2 示例代码

#### 1.2.1 视频播放控件

在需要的地方添加

```Objective-C
#import <PLPlayerKit/PLPlayerKit.h>
```

参数配置

```Objective-C
	NSMutableDictionary *parameters = [@{} mutableCopy];
	
	// 对于 iPhone 建议关闭逐行扫描，默认是开启的
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
		parameters[PLVideoParameterDisableDeinterlacing] = @(YES);
	}
	
	// 当业务需要在播放器初始化完成后自动开始播放，可以在初始化参数中设定
	parameters[PLPlayerParameterAutoPlayEnable] = @(YES);
```

初始化

```Objective-C
	// 初始化 PLVideoPlayerController
	PLVideoPlayerController *playerController = [PLVideoPlayerController videoPlayerControllerWithContentURL:url parameters:parameters];
	
	// 设定 delegate
    playerController.delegate = self;
    
	// 获取到播放界面
	UIView *playerView = playerController.playerView;
```

开始／暂停操作

```Objective-C
   // 准备播放
   // 准备播放的方法主要完成流连接，解码器初始化等工作
   [playerController prepareToPlayWithCompletion:^(BOOL success) {
       if (success) {
           [playerController play];
       }
   }];
   
	// 播放
	// 如果你没有主动的调用 prepareToPlayWithCompletion 方法, 直接调用 play 方法也是没有问题, play 方法内部会自行调用 prepareToPlayWithCompletion 方法来完成解码器初始化工作
	[playerController play];
	
	// 暂停
	[playerController pause];
	
	// 停止
	[playerController stop];
```

播放器状态获取

```
// 实现 <PLVideoPlayerControllerDelegate> 来控制流状态的变更
- (void)videoPlayerController:(PLVideoPlayerController *)controller playerStateDidChange:(PLPlayerState)state {
	// 这里会返回流的各种状态，你可以根据状态做 UI 定制及各类其他业务操作
}

- (void)videoPlayerControllerDecoderHasBeenReady:(PLVideoPlayerController *)controller {
	// 解码器初始化完成, 与 videoPlayerController:playerStateDidChange: 方法中返回 PLVideoPlayerStateReady 状态属于同一情况，你可以仅仅实现 videoPlayerController:playerStateDidChange: 方法
}

- (void)videoPlayerController:(PLVideoPlayerController *)playerController failureWithError:(NSError *)error {
	// 当出现错误时，你会在这里收到回调，暂且只有解码器初始化错误会返回
}

- (void)videoPlayerController:(PLVideoPlayerController *)playerController positionDidChange:(NSTimeInterval)position {
	// 视频进度变更时都会触发这个回调
}
```

#### 1.2.2 纯音频播放控件

在需要的地方添加

```Objective-C
#import <PLPlayerKit/PLPlayerKit.h>
```

参数配置

```Objective-C
	NSMutableDictionary *parameters = [@{} mutableCopy];
	
	// 当业务需要在播放器初始化完成后自动开始播放，可以在初始化参数中设定
	parameters[PLPlayerParameterAutoPlayEnable] = @(YES);
```

初始化

```Objective-C
	// 初始化 PLAudioPlayerController
	PLAudioPlayerController *playerController = [PLAudioPlayerController audioPlayerControllerWithContentURL:url parameters:parameters];
	
	// 设定 delegate
    playerController.delegate = self;
```

开始／暂停操作

```Objective-C
	// 准备播放
   // 准备播放的方法主要完成流连接，解码器初始化等工作
   [playerController prepareToPlayWithCompletion:^(BOOL success) {
       if (success) {
           [playerController play];
       }
   }];
   
	// 播放
	// 如果你没有主动的调用 prepareToPlayWithCompletion 方法, 直接调用 play 方法也是没有问题, play 方法内部会自行调用 prepareToPlayWithCompletion 方法来完成解码器初始化工作
	[playerController play];
	
	// 暂停
	[playerController pause];
	
	// 停止
	[playerController stop];
```

播放器状态获取

```
// 实现 <PLAudioPlayerControllerDelegate> 来控制流状态的变更
- (void)audioPlayerController:(PLAudioPlayerController *)controller playerStateDidChange:(PLPlayerState)state {
	// 这里会返回流的各种状态，你可以根据状态做 UI 定制及各类其他业务操作
}

- (void)audioPlayerControllerDecoderHasBeenReady:(PLAudioPlayerController *)controller {
	// 解码器初始化完成, 与 audioPlayerController:playerStateDidChange: 方法中返回 PLPlayerStateReady 状态属于同一情况，你可以仅仅实现 audioPlayerController:playerStateDidChange: 方法
}

- (void)audioPlayerController:(PLAudioPlayerController *)playerController failureWithError:(NSError *)error {
	// 当出现错误时，你会在这里收到回调，暂且只有解码器初始化错误会返回
}

- (void)audioPlayerController:(PLAudioPlayerController *)playerController positionDidChange:(NSTimeInterval)position {
	// 音频进度变更时都会触发这个回调
}

- (void)audioPlayerControllerWillBeginBackgroundTask:(PLAudioPlayerController *)controller {
    // 当开启了后台播放时, 进入后台时会出发后台任务的创建, 创建之前便会回调这个回调方法
}

- (void)audioPlayerController:(PLAudioPlayerController *)controller willEndBackgroundTask:(BOOL)isExpirationOccured {
    // 当开启了后台播放时, 进入后台后创建的后台任务在超时或者回到前台时，会被销毁掉，便会调用这个回调方法
}
```

#### 1.2.3 配置参数

```
// 视频播放器参数
// 逐行扫描
PLVideoParameterDisableDeinterlacing

// Player contentMode
PLVideoParameterFrameViewContentMode
```

```
// 通用配置参数，音视频均可用
// 最小缓存时长
PLPlayerParameterMinBufferedDuration

// 最大缓存时长
PLPlayerParameterMaxBufferedDuration

// 是否自动开始播放
PLPlayerParameterAutoPlayEnable
```

## 2 包含的第三方库

- ffmpeg

## 3 系统要求

- iOS Target : >= iOS 6

## 4 版本历史

- 1.2.17 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.17.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.16.md))
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