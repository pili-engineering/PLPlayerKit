# PLPlayerKit Release Notes for 2.1.0

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

- 解决了播放过程中可能出现声音消失的问题
- 解决了退后台返回后音视频无法正常同步的问题
- 修改播放器音视频同步机制
- 解决持续播放过程中出现部分内存没有正确释放的问题
- 解决了 iOS 版本小于 8.0 时 Demo 出现的crash问题
