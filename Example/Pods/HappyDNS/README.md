# Happy DNS for Objective-C

[![@qiniu on weibo](http://img.shields.io/badge/weibo-%40qiniutek-blue.svg)](http://weibo.com/qiniutek)
[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](LICENSE.md)
[![Build Status](https://travis-ci.org/qiniu/happy-dns-objc.svg?branch=master)](https://travis-ci.org/qiniu/happy-dns-objc)
[![Latest Stable Version](http://img.shields.io/cocoapods/v/HappyDNS.svg)](https://github.com/qiniu/happy-dns-objc/releases)
![Platform](http://img.shields.io/cocoapods/p/HappyDNS.svg)

## 用途

调用系统底层Dns解析库，可以使用114 等第三方dns解析，也可以集成dnspod等httpdns。另外也有丰富的hosts 域名配置。

## 安装

通过CocoaPods

```ruby
pod "HappyDNS"
```

## 运行环境


## 使用方法


## 测试


### 所有测试

``` bash
$ xctool -workspace HappyDNS.xcworkspace -scheme "HappyDNS_Mac" -sdk macosx -configuration Release test -test-sdk macosx
```

### 指定测试

可以在单元测试上修改，熟悉使用

``` bash
```

## 常见问题

- 如果碰到其他编译错误，请参考 CocoaPods 的 [troubleshooting](http://guides.cocoapods.org/using/troubleshooting.html)
- httpdns 在ios8 时不支持 nat64 模式下 IP 直接访问url，原因是 NSUrlConnection 不支持。无论是用http://119.29.29.29/d 还是http://[64:ff9b::771d:1d1d]/d 都不行，此时可以使用localdns方式。

## 代码贡献

详情参考[代码提交指南](https://github.com/qiniu/happy-dns-objc/blob/master/CONTRIBUTING.md)。

## 贡献记录

- [所有贡献者](https://github.com/qiniu/happy-dns-objc/contributors)

## 联系我们

- 如果有什么问题，可以到问答社区提问，[问答社区](http://qiniu.segmentfault.com/)
- 如果发现了bug， 欢迎提交 [issue](https://github.com/qiniu/happy-dns-objc/issues)
- 如果有功能需求，欢迎提交 [issue](https://github.com/qiniu/happy-dns-objc/issues)
- 如果要提交代码，欢迎提交 pull request
- 欢迎关注我们的[微信](http://www.qiniu.com/#weixin) [微博](http://weibo.com/qiniutek)，及时获取动态信息。

## 代码许可

The MIT License (MIT).详情见 [License文件](https://github.com/qiniu/happy-dns-objc/blob/master/LICENSE).
