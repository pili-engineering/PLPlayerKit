# PLPlayerKit Release Notes for 1.2.10

## 内容

- [简介](#简介)
- [问题反馈](#问题反馈)
- [记录](#记录)
	
## 简介

PLPlayerKit 为 iOS 开发者提供直播播放 SDK。

## 问题反馈

当你遇到任何问题时，可以通过在 GitHub 的 repo 提交 ```issues``` 来反馈问题，请尽可能的描述清楚遇到的问题，如果有错误信息也一同附带，并且在 ```Labels``` 中指明类型为 bug 或者其他。

[通过这里查看已有的 issues 和提交 Bug](https://github.com/pili-io/PLPlayerKit/issues)

## 记录

### 播放器状态

- 添加对应用状态的判断，减少因进入后台通知延时未能及时暂停播放导致的 crash