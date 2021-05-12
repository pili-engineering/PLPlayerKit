# PLPlayerKit Release Notes for 3.4.5

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

- 3.4.5 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.4.5.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.4.5.md))

- 缺陷
        - 修复播放器播放aac纯音频总时间不准确
        - 修复快速切换播放地址一段时间后崩溃
        - 修复h264 split 不正确, 硬解奔溃，软解花屏
        - 修复sei 信息解析越界问题
        - 修复打点信息重复上传
        - 修复打点信息 dns 解析耗时计算不正确
        - 优化设置超时过短有可能导致的 rtsp 流打开失败的问题
        - 修复 FFmpeg 检测漏洞的问题
        - 修复hls加密文件的秘钥前缀是//无法播放
        - 修复播放未开播的直播资源，播放器无限制重试不符合预期
        - 播放完成后，切换后台再切到前台视频画面变黑屏
