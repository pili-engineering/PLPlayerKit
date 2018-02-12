# PLPlayerKit Release Notes for 3.2.1

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
   - 支持视频裁剪播放
   - 支持自定义缓存地址扩展名
   - 支持播放音量放大
   - 支持播放预加载
   - 支持点播循环播放
- 缺陷
  - 修复播放断网 crash 的问题
  - 修复 mp4 开始缓存时重连失败的问题
  - 修复快速切换打开链接的 crash 问题
  - 完善视频画面渲染色彩度
  - 修复变速后声调发生改变的问题
  - 修复 seek 后跳转不精确的问题
  - 修复视频未依据 DAR 比例显示的问题
  - 修复退后台偶现 SIGPIPE 的问题
  - 修复部分 flv 直播流卡住的问题
  - 修复 SEI 数据丢失的问题
  - 修复 videoToolbox 硬解码视频角度未矫正的问题
- 其他
  - 以动态库的方式发布，仅支持 iOS 8.0 及以上系统