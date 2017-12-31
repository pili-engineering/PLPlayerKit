# PLPlayerKit Release Notes for 3.2.0

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
  - 支持 flv 使用 mp3 音频格式
  - 支持 http 的 DNS 异步解析
  - 支持视频根据旋转角度自动旋转
- 缺陷
  - 修复失去音频第一帧渲染问题
  - 修复 OpenGL crash 的问题
  - 修复部分视频音画不同步的问题
  - 修复部分视频花屏、马赛克的问题
  - 修复弱网播放偶现快速切换卡顿的问题
  - 修复进入后台切换第三方应用 crash 的问题
  - 解决由第三方引起的 ffmpeg 冲突问题
  - 修复直播 url 中含有 ?domain= 无法播放的问题
  - 修复音频视频时长不匹配 resume 播放失败的问题
  
