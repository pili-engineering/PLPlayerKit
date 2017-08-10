# PLPlayerKit Release Notes for 3.0.0

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

- 全面升级为七牛自研的播放器内核，拥有更优异的性能
- 功能
  - 新增 HLS 七牛私有 DRM 的支持
  - 新增 H.265 格式的播放
  - 新增点播倍速播放
  - 支持点播 mp4 视频本地缓存播放
- 优化
  - 优化包体大小
  - 优化 CPU、内存和功耗
  - 优化直播模式下的追帧策略，效果更加平滑
  - 优化重连逻辑，不用销毁播放器，网络断开后内部自动重连
  - 优化 mp4 点播，使用双 IO 技术更高效地播放 moov 在尾部的 mp4 文件
