# PLPlayerKit Release Notes for 1.2.19

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

### Bug

- 修复播放无返回状态的问题（针对无直播的流、hls 回放）
- 修复 hls 回放结束时无 stopped 回调的问题
- 修复 hls 回放开始的 duration 不为 0 的问题
