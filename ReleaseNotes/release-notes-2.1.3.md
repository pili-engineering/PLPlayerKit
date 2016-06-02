# PLPlayerKit Release Notes for 2.1.3

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

- 增加设置一级缓存和二级缓存的选项，便于控制卡顿率
- 修复播放 OBS 及 FFmpeg 推的流黑屏的问题
- 修复播放结束后无法重播的问题
- 修复播放过程中内存暴增的问题
- 拆分 pili-librtmp 为公共依赖，解决模拟器环境下与 PLStreamingKit 冲突的问题
