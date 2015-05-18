# PLPlayerKit

PLPlayerKit 是为 **pili流媒体云服务** 提供的一套播放直播流的 SDK, 旨在解决 iOS 端快速、轻松实现 iOS 设备播放直播流，便于 **pili流媒体云服务** 的开发者专注于产品业务本身，而不必在技术细节上花费不必要的时间。


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
pod install
```

- Done! 运行你工程的 workspace

### 1.2 示例代码

#### 1.2.1 可定制的播放控件

在需要的地方添加

```Objective-C
#import <PLPlayerKit/PLPlayerKit.h>
```

初始化

```Objective-C
	// 初始化 PLVideoPlayerController
	PLVideoPlayerController *playerController = [PLVideoPlayerController videoPlayerControllerWithContentURL:url parameters:parameters];
	
	// 获取到播放界面
	UIView *playerView = playerController.playerView;
```

参数配置

```Objective-C
	NSMutableDictionary *parameters = [@{} mutableCopy];
	
	// 对于 iPhone 建议关闭逐行扫描，默认是开启的
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
		parameters[PLMovieParameterDisableDeinterlacing] = @(YES);
	}
```

开始／暂停操作
```Objective-C
	// 播放
	[playerController play];
	
	// 停止
	[playerController pause];
```

## 2 包含的第三方库

- ffmpeg

## 3 系统要求

- iOS Target : >= iOS 6

## 4 版本历史

- 1.2.2 ([Release Notes](https://github.com/pili-io/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.2.md) && [API Diffs](https://github.com/pili-io/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.2.md))
	- 修复 lib 未更新导致的 crash
- 1.2.1 ([Release Notes](https://github.com/pili-io/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.1.md) && [API Diffs](https://github.com/pili-io/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.1.md))
	- 添加 failue 情况下的回调，返回 NSError 对象
	- 移除 PLVideoPlayerViewController，请直接使用 PLVideoPlayerController 进行定制
- 1.2.0 ([Release Notes](https://github.com/pili-io/PLPlayerKit/blob/master/ReleaseNotes/release-notes-1.2.0.md) && [API Diffs](https://github.com/pili-io/PLPlayerKit/blob/master/APIDiffs/api-diffs-1.2.0.md))
	- 极大缩小 lib 大小
	- 增加可定制的播放控件 PLVideoPlayerController
- 1.1.2
	- 拆分 Flat lib
	- 添加了 x86_64 支持，便于在 iPhone 6 Plus 模拟器下调试使用
- 1.1.1
	- 对库引用做了些修改
- 1.1.0
	- 发布 CocoaPods 版本