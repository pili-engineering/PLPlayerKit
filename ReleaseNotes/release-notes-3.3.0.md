# PLPlayerKit Release Notes for 3.3.0

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

- 3.3.0 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.3.0.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.3.0.md))
- 功能
   - 支持视频流宽高的回调
   - 支持下载暂停功能
   - 支持获取缓冲文件的长度
   - 支持 seekTo 完成的回调
- 缺陷
  - 修复内存泄漏的问题
  - 修复获取音量值不够精确的问题
  - 修复循环播放回退及失败的问题
  - 修复特定流打开时间较长的问题
  - 修复特定链接播放有声音没画面的问题
  - 修复设备拍摄视频未修正宽高的问题 
  - 修复纯音频 seekTo 失败的问题
  - 修复重连成功后状态未改变的问题
  - 修复暂停后 PLPlayerStatus 状态错误的问题
  - 修复视频播放后设置 playerView 的 contentMode 无效的问题
- 其他
  - 大幅降低 MP4 长视频首开时间，20M 网络下，2 小时 MP4 文件首开只需 0.6 - 1秒
  - 去除 HappyDNS，使用内置的 DNS 解析