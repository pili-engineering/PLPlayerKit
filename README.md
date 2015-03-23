# PLPlayerKit

PLPlayerKit 是为 **pili 流媒体云服务** 提供的一套播放直播流的 SDK, 旨在解决 iOS 端快速、轻松实现 iOS 设备播放直播流，便于 **pili 流媒体云服务** 的开发者专注于产品业务本身，而不必在技术细节上花费不必要的时间。


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

在需要的地方添加

```Objective-C
#import <PLPlayerKit/PLPlayerKit.h>
```

初始化

```Objective-C
	// 初始化 VideoPlayerViewController
	PLVideoPlayerViewController *viewPlayerViewController = [PLVideoPlayerViewController videoPlayerViewControllerWithContentURL:url parameters:parameters];
	
	// 展示播放界面
	[self presentViewController:viewPlayerViewController animated:YES completion:nil];
```

参数配置

```Objective-C
	NSMutableDictionary *parameters = [@{} mutableCopy];
	
	// 对于 iPhone 建议关闭逐行扫描，默认是开启的
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
		parameters[PLMovieParameterDisableDeinterlacing] = @(YES);
	}
```

播放操作，PLVideoPlayerViewController 会在展示时自动开始播放，当然，如果你需要自己在代码中控制播放逻辑，也可以调用以下方法轻松开始／暂停

```Objective-C
	// 播放
	[viewPlayerViewController play];
	
	// 停止
	[viewPlayerViewController pause];
```

如果你想自定义播放界面，那么你需要隐藏原有的播放控制，你可以这么做到

## 2 包含的第三方库

- ffmpeg
- kxmovie

## 3 系统要求

- iOS Target : >= iOS 6

## 4 版本历史
- 1.1.2
	- 添加了 x86_64 支持，便于在 iPhone 6 Plus 模拟器下调试使用
- 1.1.1
	- 对库引用做了些修改
- 1.1.0
	- 发布 CocoaPods 版本