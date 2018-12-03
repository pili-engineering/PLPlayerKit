# PLPlayerKit Release Notes for 3.4.0

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