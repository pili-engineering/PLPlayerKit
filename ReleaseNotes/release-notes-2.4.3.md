# PLPlayerKit Release Notes for 2.4.3

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
  - 新增流分辨率变化的通知
  - 新增提供更多音视频信息的回调接口
  - 新增首开耗时接口
  - 增强 FFmpeg 点播硬解兼容性
- 缺陷
  - 修复 AVPlayer 点播 pause 状态切换时播放器状态异常的问题
  - 修复 FFmpeg 点播纯音频流时 seek 失败的问题
  - 修复硬解在某些场景下出现绿屏的问题
