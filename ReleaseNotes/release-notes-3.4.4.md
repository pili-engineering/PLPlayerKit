# PLPlayerKit Release Notes for 3.4.4

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

- 3.4.4 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.4.4.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.4.4.md))

- 缺陷
	- 修复硬解码画面比例有概率错误的问题
	- 修复硬解码特定视频内存上涨的问题
	- 修复带端口号的 hls 播放失败的问题
	- 修复开启缓存特定视频打开失败的问题
	- 修复开启缓存特定视频没有声音的问题
	- 修复纯音频播放偶现 crash 的问题
	- 修复弱网下特定视频卡死的问题
	- 修复特定视频 seek 内存泄漏的问题