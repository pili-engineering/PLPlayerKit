# PLPlayerKit Release Notes for 2.1.1

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

- 首屏开启速度优化，在网络状况良好的情况下能实现秒开效果
- 弱网情况下的累积延迟问题优化，较好控制累积延迟在数秒以内
- 解决了上一版遇到的无法设置 playerView.contentMode 以及 playerOption 的问题
- 解决了不标准流可能出现的音频断续，播放器内存异常增长问题
- 后台播放体验优化，修复了后台播放被其他音频打断后出现的一系列问题
- 解决了应用切换时出现的 UI 卡死问题
