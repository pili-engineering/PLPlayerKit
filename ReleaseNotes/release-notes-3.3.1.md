# PLPlayerKit Release Notes for 3.3.1

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

- 3.3.1 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.3.1.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.3.1.md))
- 功能
   - 支持调用 seekTo 后回调结果
   - 支持提前设置 mp4 解析时间
   - 支持提前设置点播从记忆点开始播放
   - 支持内部自动检查格式的功能
- 缺陷
   - 修复 pause 后调整预览 size 导致画面拉伸或压缩的问题
   - 修复开启离线缓存后，偶现无法循环播放的问题
   - 修复 4G 网下 iPv6 解析错误的问题
   - 修复特殊情况下多次创建释放 player 对象导致崩溃的问题
   - 修复截图功能失效的问题
   - 修复弱网环境调用 stop 卡主线程的问题
   - 修复重连崩溃或失败的问题
   - 解决系统 DNS 部分不可用的问题
   - 修复本地 m3u8 文件无法正常播放的问题
   - 修复 open fail 之后 destroy player 的 crash 问题
   - 修复未获取总时长，回调 loadTimeRange 一直为 0 的问题
   - 修复某些长视频 seek 操作异常的问题
   - 修复点播播放完成后，resume 或 seek 至文件开头，播放状态错误的问题
   - 修复优化 mp4 快开带来的一些问题
- 其他
   - 优化下载数据时内存的使用
   - 去除 `PLPlayerStatus`的两个值`PLPlayerStatusSeeking` 及 `PLPlayerStatusSeekFailed`
   - 修改缓冲回调参数`CMTimeRange`为`CMTime`
   - 修改 `preDNSHost` 类方法为实例方法
