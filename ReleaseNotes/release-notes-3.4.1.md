# PLPlayerKit Release Notes for 3.4.1

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