# PLPlayerKit Release Notes for 2.0.2

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

- 添加 RTMP Cache 机制
- 添加数据超时属性
- 修复 RTMP 播放内存 leak
- 修复 RTMP 播放音频错误问题
- 修复 RTMP 播放主线程卡死问题
- 优化架构，减少内存和 cpu 占用
