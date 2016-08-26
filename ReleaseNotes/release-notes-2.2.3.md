# PLPlayerKit Release Notes for 2.2.3

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
  - 新增 QoS 功能
  - 新增渲染数据回调，该功能只支持直播
  - 新增截图功能，该功能只支持直播
  - 新增 MP3 后台播放
  

- 缺陷
  - 修复后台播放时，触发超时重连，丢失 sps/pps，回到前台画面停住，声音正常的问题
  - 修复 RTMP 扩展时间戳的问题
  - 修复播放器释放阻塞主线程的问题
  - 优化音视频同步机制
  - 优化 caching 状态检查
