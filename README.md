# PLPlayerKit

PLPlayerKit 是一个适用于 iOS 的音视频播放器 SDK，可高度定制化和二次开发，特色是支持 RTMP, HTTP-FLV 和 HLS 直播流媒体播放。

SDK 的 Github 地址：https://github.com/pili-engineering/PLPlayerKit

功能特性

- [x] 高可定制
- [x] 直播累积延迟消除技术
- [x] 支持首屏秒开
- [x] 支持 RTMP 直播流播放
- [x] 支持 HTTP-FLV 直播流播放
- [x] 支持 HLS 播放
- [x] 支持 HTTPS 播放
- [x] 支持多种画面预览模式
- [x] 支持画面旋转与镜像
- [x] 支持播放器音量设置
- [x] 支持纯音频播放
- [x] 支持后台播放
- [x] 支持使用 IP 地址的 URL 
- [x] 支持软硬解自动切换
- [x] 支持 H.265 格式播放
- [x] 支持 HLS 七牛私有 DRM
- [x] 支持点播倍速播放
- [x] 支持点播 mp4 视频本地缓存播放
- [x] 支持 SEI 数据回调
- [x] 支持 rtmp 原始时间戳 
- [x] 支持 flv 使用 mp3 音频格式
- [x] 支持 http 的 DNS 异步解析
- [x] 支持视频根据旋转角度自动旋转
- [x] 支持视频裁剪播放
- [x] 支持自定义缓存地址扩展名
- [x] 支持播放音量放大
- [x] 支持播放预加载
- [x] 支持点播循环播放
- [x] 支持下载暂停功能
- [x] 支持获取缓冲文件的长度
- [x] 支持 seekTo 完成的回调
- [x] 支持提前设置 mp4 解析时间
- [x] 支持提前设置点播从记忆点播放
- [x] 支持 ffconcat

## 说明

从 **v3.0.0** 开始，SDK 全面升级为七牛完全自研的播放器内核，拥有更加优异的性能，升级内容如下：

- [x] 新增倍数播放功能（0.5x，1x，2x，4x 等）
- [x] 新增 mp4 本地缓存功能
- [x] 新增 HLS 七牛私有 DRM 的支持 
- [x] 新增 H.265 格式播放的支持
- [x] 优化 CPU、内存和功耗
- [x] 优化首开效果，首开速度有大幅提升
- [x] 优化重连逻辑，不用销毁播放器，网络断开后内部自动重连
- [x] 优化 mp4 点播，使用双 IO 技术更高效地播放 moov 在尾部的 mp4 文件
- [x] 支持播放过程中变速不变调，可实现更平滑的追帧效果，更少的卡顿率
- [x] 新增支持 SEI 数据回调，更多数据交互
- [x] 新增音视频渲染首帧回调，更便捷
- [X] 新增预加载切换 URL 功能

如果从旧版本升级，建议参考[升级指南](https://github.com/pili-engineering/PLPlayerKit/blob/master/v3.x.x-UpgradeGuide.md)以及[ReleaseNotes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes)文件夹下的各个版本说明，查看注意事项

## 内容摘要

- [快速开始](#1-快速开始)
	- [配置工程](#配置工程)
	- [示例代码](#示例代码)
- [版本历史](#版本历史)

## 快速开始

### 配置工程

#### CocoaPods 导入

- 配置你的 Podfile 文件，默认真机，添加如下配置信息

```
pod 'PLPlayerKit'
```
- 若需要使用模拟器 + 真机，则改用如下配置

```
pod "PLPlayerKit", :podspec => 'https://raw.githubusercontent.com/pili-engineering/PLPlayerKit/master/PLPlayerKit-Universal.podspec'
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

**注意：鉴于目前 iOS 上架，只支持动态库真机，请在 App 上架前，更换至真机版本**

#### 手动导入  

- 根据需要，将 Pod 目录下真机或真机 + 模拟器版本的 framework 文件加入到工程中；
- Build Setting 下 Other Linker Flags 中添加 -ObjC
- Build Phases 下 Link Binary With Libraries 中添加如图所示
![](http://sdk-release.qnsdk.com/PLPLayerKit.jpg)

### 添加相关配置

添加 App Transport Security Setting

* 如图所示

![](http://7xuil4.com1.z0.glb.clouddn.com/permession.jpg)


配置 Required background modes

* 如图所示

![](http://7xuil4.com1.z0.glb.clouddn.com/AllowBackground-4.png)

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
[option setOptionValue:@2000 forKey:PLPlayerOptionKeyMaxL1BufferDuration];
[option setOptionValue:@1000 forKey:PLPlayerOptionKeyMaxL2BufferDuration];
[option setOptionValue:@(NO) forKey:PLPlayerOptionKeyVideoToolbox];
[option setOptionValue:@(kPLLogInfo) forKey:PLPlayerOptionKeyLogLevel];

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
  // 开始播放，当连接成功后，将收到第一个 PLPlayerStatusCaching 状态
  // 第一帧渲染后，将收到第一个 PLPlayerStatusPlaying 状态
  // 播放过程中出现卡顿时，将收到 PLPlayerStatusCaching 状态
  // 卡顿结束后，将收到 PLPlayerStatusPlaying 状态
}

- (void)player:(nonnull PLPlayer *)player stoppedWithError:(nullable NSError *)error {
	// 当发生错误，停止播放时，会回调这个方法
}

- (void)player:(nonnull PLPlayer *)player codecError:(nonnull NSError *)error {
  // 当解码器发生错误时，会回调这个方法
  // 当 videotoolbox 硬解初始化或解码出错时
  // error.code 值为 PLPlayerErrorHWCodecInitFailed/PLPlayerErrorHWDecodeFailed
  // 播发器也将自动切换成软解，继续播放
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

**注意：请确保 `isPlayable` 为 YES，即可以播放；`canPlayInBackground` 为 YES，即可以后台播放。**

达到 `isPlayable` 以及 `canPlayInBackground` 两者为 YES，示例设置代码如下：

```Objective-C
[[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
[[AVAudioSession sharedInstance] setActive:YES error:nil];
```

## 其它依赖库版本号
- FFmpeg : v3.3.x
- OpenSSL: OpenSSL_1_1_0f
- Speex: v1.2.0

## 版本历史
- 3.4.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.4.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.4.1.md))

- 功能 
	- HLS 支持 G711
	- 支持 HTTPS 异步连接 
	- 支持 DRM 解密

- 缺陷
	- 修复重连时间过长的问题
	- 修复硬解码的 crash 问题 
	- 修复偶现 dealloc 调用 weak self 的 crash 问题

- 优化
	- 改善 RTMP 的重连速度
	- 改善快速切换 URL 时 API 的调用耗时

- 3.4.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.4.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.4.0.md))

- 功能
	- 支持用户代理设置
	- 支持 FMP4 格式 
	- 添加 IO 缓存功能

- 缺陷
	- 修复使用硬解码的时候截图崩溃问题
	- 修复使用硬解码的时候 seek 错误的问题
	- 修复播放 HTTP-FLV 直播流重连失败的问题 
	- 修复播放 FLV 时间戳错误问题
	- 修复部分 MP4 文件循环播放失败的问题
- 优化
	- 去掉对 i386 模拟器的支持，减小 SDK 体积

- 3.3.3 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.3.3.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.3.3.md))
- 功能
   - 支持真机 bitcode
- 缺陷
   - 修复清理缓存的资源后播放 open failed 的问题
   - 修复部分视频播放卡顿或卡顿兼加速播放的问题
   - 修复特殊视频播放只有声音没有图像的问题
   - 修复 flv 格式下播放内存增长较快 
   - 修复 COpenGLRnd Render 渲染的 crash 问题
   - 修复裁剪画面并旋转后画面错位的问题 
- 3.3.2 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.3.2.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.3.2.md))
- 功能
   - 支持 ffconcat 文件
- 缺陷
   - 修复弱网下重新打开链接的 crash 问题
   - 修复特殊场景下调用 pause 偶现卡死的问题
   - 修复特殊场景下缓存 mp4 失败的问题
   - 修复部分直播视频偶现经常 caching 的问题
   - 修复部分长视频 mp4 调用 seekTo 后一直 buffering 问题
   - 修复从 YUV 到 RGB 的颜色差别问题
- 其他
   - 增加硬解保护
   - 提高首帧的打开速度
   - 内部支持分析 DNS 服务器设置
   - 改进点播音频文件不从 0 开始播放的问题
- 3.3.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.3.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.3.1.md))
- 功能
   - 支持调用 seekTo 后回调结果
   - 支持提前设置 mp4 解析时间
   - 支持提前设置点播从记忆点开始播放
   - 支持内部自动检查格式的功能
- 缺陷
   - 修复 pause 后调整预览 size 导致画面拉伸或压缩的问题
   - 修复开启离线缓存后，偶现无法循环播放的问题
   - 修复 4G 网下 iPv6 解析错误的问题
   - 修复特殊情况下多次创建释放 player 对象导致崩溃的问题
   - 修复截图功能失效的问题
   - 修复弱网环境调用 stop 卡主线程的问题
   - 修复重连崩溃或失败的问题
   - 解决系统 DNS 部分不可用的问题
   - 修复本地 m3u8 文件无法正常播放的问题
   - 修复 open fail 之后 destroy player 的 crash 问题
   - 修复未获取总时长，回调 loadTimeRange 一直为 0 的问题
   - 修复某些长视频 seek 操作异常的问题
   - 修复点播播放完成后，resume 或 seek 至文件开头，播放状态错误的问题
   - 修复优化 mp4 快开带来的一些问题
- 其他
   - 优化下载数据时内存的使用
   - 去除 `PLPlayerStatus`的两个值`PLPlayerStatusSeeking` 及 `PLPlayerStatusSeekFailed`
   - 修改缓冲回调参数`CMTimeRange`为`CMTime`
   - 修改 `preDNSHost` 类方法为实例方法
- 3.3.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.3.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.3.0.md))
- 功能
   - 支持视频流宽高的回调
   - 支持下载暂停功能
   - 支持获取缓冲文件的长度
   - 支持 seekTo 完成的回调
- 缺陷
  - 修复内存泄漏的问题
  - 修复获取音量值不够精确的问题
  - 修复循环播放回退及失败的问题
  - 修复特定流打开时间较长的问题
  - 修复特定链接播放有声音没画面的问题
  - 修复设备拍摄视频未修正宽高的问题 
  - 修复纯音频 seekTo 失败的问题
  - 修复重连成功后状态未改变的问题
  - 修复暂停后 PLPlayerStatus 状态错误的问题
  - 修复视频播放后设置 playerView 的 contentMode 无效的问题
- 其他
  - 大幅降低 MP4 长视频首开时间，20M 网络下，2 小时 MP4 文件首开只需 0.6 - 1秒
  - 去除 HappyDNS，使用内置的 DNS 解析
- 3.2.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.2.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.2.1.md))
- 功能
   - 支持视频裁剪播放
   - 支持自定义缓存地址扩展名
   - 支持播放音量放大
   - 支持播放预加载
   - 支持点播循环播放
- 缺陷
  - 修复播放断网 crash 的问题
  - 修复 mp4 开始缓存时重连失败的问题
  - 修复快速切换打开链接的 crash 问题
  - 完善视频画面渲染色彩度
  - 修复变速后声调发生改变的问题
  - 修复 seek 后跳转不精确的问题
  - 修复视频未依据 DAR 比例显示的问题
  - 修复退后台偶现 SIGPIPE 的问题
  - 修复部分 flv 直播流卡住的问题
  - 修复 SEI 数据丢失的问题
  - 修复 videoToolbox 硬解码视频角度未矫正的问题
- 其他
  - 以动态库的方式发布，仅支持 iOS 8.0 及以上系统
- 3.2.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.2.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.2.0.md))
- 功能
  - 支持 flv 使用 mp3 音频格式
  - 支持 http 的 DNS 异步解析
  - 支持视频根据旋转角度自动旋转
- 缺陷
  - 修复失去音频第一帧渲染问题
  - 修复 OpenGL crash 的问题
  - 修复部分视频音画不同步的问题
  - 修复部分视频花屏、马赛克的问题
  - 修复弱网播放偶现快速切换卡顿的问题
  - 修复进入后台切换第三方应用 crash 的问题
  - 解决由第三方引起的 ffmpeg 冲突问题
  - 修复直播 url 中含有 ?domain= 无法播放的问题
  - 修复音频视频时长不匹配 resume 播放失败的问题
- 3.1.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.1.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.1.0.md))
- 功能
  - 支持读取 mp3, aac 格式
  - 支持读取 rtmp 流中的音视频时间戳信息
  - 支持读取视频旋转角度参数
  - 支持 mpeg 格式播放
- 缺陷
  - 修复 Stop 时偶现的 Crash 及卡住的问题
  - 修复 ipv6 rtmp 无法播放的问题
  - 修复播放纯音频/纯视频流时得不到 playing 状态的问题
  - 修复特定 flv 流重复播放的问题
  - 修复偶现 OpenGL crash 的问题
- 3.0.2 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.0.2.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.0.2.md))
- 功能
  - 加长 URL 设置长度
- 缺陷
  - 修复 iPhone X 模拟器崩溃问题
  - 修复数据缓存回调总时长出错问题
  - 修复截图功能无效问题
  - 修复 OpenGL 崩溃问题
  - 修复无法修改 playerView 的 bounds 属性的问题
- 3.0.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.0.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.0.1.md))
- 功能
  - 新增 SEI 数据回调
  - 新增播放格式预设置
  - 新增同格式快速播放接口
- 缺陷
  - 修复播放器错误时收不到 error 状态回调的问题
  - 修复某些 mp4 无法播放的问题
  - 修复多次 stop 时 crash 的问题
- 3.0.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.0.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.0.0.md))
- 全面升级为七牛自研的播放器内核，拥有更优异的性能
- 功能
  - 新增 HLS 七牛私有 DRM 的支持
  - 新增 H.265 格式的播放
  - 新增点播倍速播放
  - 新增点播 mp4 视频缓存播放
- 优化
  - 优化包体大小
  - 优化 CPU、内存和功耗
  - 优化直播模式下的追帧策略，效果更加平滑
  - 优化重连逻辑，不用销毁播放器，网络断开后内部自动重连
  - 优化 mp4 点播，使用双 IO 技术更高效地播放 moov 在尾部的 mp4 文件
- 2.4.3 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.4.3.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.4.3.md))
- 功能
  - 新增流分辨率变化的通知
  - 新增提供更多音视频信息的回调接口
  - 新增首开耗时接口
  - 增强 FFmpeg 点播硬解兼容性
- 缺陷
  - 修复 AVPlayer 点播 pause 状态切换时播放器状态异常的问题
  - 修复 FFmpeg 点播纯音频流时 seek 失败的问题
  - 修复硬解在某些场景下出现绿屏的问题
- 2.4.2 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.4.2.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.4.2.md))
- 缺陷
  - 修复 AVPlayer 播放时调用 pause 和设置 frame 无效的问题
  - 修复解码器释放时线程并发导致的偶发 crash
- 2.4.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.4.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.4.1.md))
- 功能
  - 新增 probesize 参数配置
  - 新增播放器初始化后更新 URL 的接口
  - 新增 AVPlayer 点播的缓冲进度接口
  - 增加 http header 中 referer 自定义接口
- 缺陷
  - 修复锁屏且屏幕黑后，播放没有声音的问题
  - 修复播放器释放时偶发的 crash
- 2.4.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.4.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.4.0.md))
- 功能
  - 新增 https 支持
  - 新增文件播放
  - 新增 speex, ogg 等音视频格式， avi, m4a 等封装格式支持。
  - 新增 display aspect ratio 信息
  - 新增 DNS 预解析接口
  - 新增开播前封面图
- 缺陷
  - 修复一些偶发的 crash
- 2.3.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.3.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.3.0.md))
- 功能
  - 新增直播流画面旋转模式
  - 新增直播流分辨率信息
  - 新增停止渲染的选项
  - 新增基于 FFMPEG 的点播
- 缺陷
  - 修复一些偶现的 crash
- 优化
  - 优化开始播放的快进时间
- 2.2.4 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.2.4.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.2.4.md))
- 缺陷
  - 修复与 CocoaLumberjack 符号冲突的问题
- 2.2.3 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.2.3.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.2.3.md))
- 功能
  - 新增 QoS 功能
  - 新增渲染数据回调
  - 新增截图功能
  - 新增 MP3 后台播放
- 缺陷
  - 修复后台播放时，触发超时重连，丢失 sps/pps，回到前台画面停住，声音正常的问题
  - 修复 RTMP 扩展时间戳的问题
  - 修复播放器释放阻塞主线程的问题
  - 优化音视频同步机制
  - 优化 caching 状态检查
- 2.2.2 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.2.2.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.2.2.md))
- 功能
  - 新增 AAC HEV2 音频支持
  - 新增 SDK 自动重连功能，默认不开启
- 缺陷
  - 修复长时间播放偶发解码 crash
  - 修复 pause/resmue 快速调用导致 crash
  - 修复重连未更换服务器 IP
  - 修复 rtmp 硬解播放视频抖动
  - 修复 flv 开始播放偶发黑屏
  - 修复 flv 超时机制失效
- 2.2.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-2.2.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-2.2.1.md))
- 功能
  - 支持 SDK 日志级别设置
  - 新增 HappyDNS 支持
- 缺陷
  - 修复回看状态不准确问题
  - 修复跳转第三方应用，出现内存增加
  - 修复播放卡住 caching 状态
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
