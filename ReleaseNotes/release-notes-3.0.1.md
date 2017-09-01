# PLPlayerKit Release Notes for 3.0.1

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
  - 新增 SEI 数据回调
  - 新增播放格式预设置
  - 新增同格式快速播放接口
- 缺陷
  - 修复播放器错误时收不到 error 状态回调的问题
  - 修复某些 mp4 无法播放的问题 
  - 修复多次 stop 时 crash 的问题
