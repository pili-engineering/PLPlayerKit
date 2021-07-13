# PLPlayerKit Release Notes for 3.4.6

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

- 3.4.6 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.4.6.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.4.6.md))

- 新增
     - 视频预缓存功能
     - 本地缓存目录加密功能 
- 缺陷
     - 修复 SEI 数据回调时，数组越界导致崩溃。
     - 修复打开视频流数据回调时，视频流 fromat 改变导致概率性 crash 的问题
     - 修复视频流的 NAL 头格式不一致时，SEI 数据没有回调 

     
