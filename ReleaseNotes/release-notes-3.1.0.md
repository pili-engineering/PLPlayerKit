# PLPlayerKit Release Notes for 3.1.0

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
