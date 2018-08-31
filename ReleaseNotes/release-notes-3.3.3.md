# PLPlayerKit Release Notes for 3.3.3

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
- 3.3.3 ([Release Notes](https://github.com/pili-engineering/PLPlayerKit/blob/master/ReleaseNotes/release-notes-3.3.3.md) && [API Diffs](https://github.com/pili-engineering/PLPlayerKit/blob/master/APIDiffs/api-diffs-3.3.3.md))
- 功能
   - 支持真机 bitcode
- 缺陷
   - 修复清理缓存的资源后播放 open failed 的问题
   - 修复部分视频播放卡顿或卡顿兼加速播放的问题
   - 修复特殊视频播放只有声音没有图像的问题
   - 修复 flv 格式下播放内存增长较快 
   - 修复 COpenGLRnd Render 渲染的 crash 问题
   - 修复裁剪画面并旋转后画面错位的问题 