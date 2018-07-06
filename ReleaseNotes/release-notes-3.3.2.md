# PLPlayerKit Release Notes for 3.3.2

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
