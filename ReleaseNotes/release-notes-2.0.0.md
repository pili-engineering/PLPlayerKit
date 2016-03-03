# PLPlayerKit Release Notes for 2.0.0

## 内容

- [简介](#简介)
- [问题反馈](#问题反馈)
- [记录](#记录)

## 简介

PLPlayerKit 为 iOS 开发者提供直播播放 SDK。

## 问题反馈

当你遇到任何问题时，可以通过在 GitHub 的 repo 提交 ```issues``` 来反馈问题，请尽可能的描述清楚遇到的问题，如果有错误信息也一同附带，并且在 ```Labels``` 中指明类型为 bug 或者其他。

[通过这里查看已有的 issues 和提交 Bug](https://github.com/pili-engineering/PLPlayerKit/issues)

## 记录

### Player

- 添加全新的 `PLPlayer` 音视频播放控件
- RTMP 直播流后台模式支持
	- 后台 播放 RTMP 音视频流时，进入后台后声音继续播放，返回前台追帧显示最新视频帧
- 针对 RTMP 直播彻底优化
	- 首屏秒开
	- 最小化缓存延时确保直播实时性
- 去除 ffmpeg 依赖
	- 总体积减少 83%，由 67.2MB 缩减到 11.5MB(包括 armv7, armv7s, arm64, i386, x86_64，工程占用非编译后占用)
- 优化资源占用，比 1.x 版本内存占用减少 50% 以上
