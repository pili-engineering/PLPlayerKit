//
//  UIView+Alert.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <JGProgressHUD.h>
#import <MMMaterialDesignSpinner.h>

@interface UIView (Alert)
<
JGProgressHUDDelegate
>

//! 撑满view加载{}
- (void)showFullLoading;
- (void)hideFullLoading;
- (void)showFullLoadingWithTip:(NSString *)tip;

//!错误信息view{}
- (void)showErrorWithTip:(NSString *)tip tipImage:(NSString *)imageName buttonTitle:(NSString *)title retryAction:(void (^)(void))block;
//!刷新重试view{}
- (void)showRetryWithTip:(NSString *)tip retryAction:(void (^)(void))block;
- (void)hideErroMsgView;

//! 加载{图片}
- (void)showImageLoadingWithTip:(NSString *)tip image:(NSString *)imageName;

//! 加载{默认加载}
- (void)showNormalLoadingWithTip:(NSString *)tip;

//! 加载{默认加载：努力加载中}
- (void)showLoadingHUD;

//! 可取消加载{文字+默认加载}
- (void)showNormalLongLoadingWithTip:(NSString *)tip cancelTip:(NSString *)cancelTip;

//! 进度加载
- (void)showProgressLoadingWithTip:(NSString *)tip;

//! 加载成功（+文字）
- (void)showSuccessTip:(NSString *)tip;

//! 加载失败（+文字）
- (void)showFailTip:(NSString *)tip;

//! 隐藏加载
- (void)hiddenLoading;

- (void)hideLoadingHUD;

#pragma mark--Warming

//! 警告（标题+副标题+默认图片）
- (void)showWarningTip:(NSString *)title message:(NSString *)message;

//! 警告（文字+默认图片）
- (void)showWarningTip:(NSString *)tip;

//! 提示（只文字）
- (void)showTip:(NSString *)tip;

//! 提示（只文字+位置）
- (void)showTip:(NSString *)tip position:(JGProgressHUDPosition)position;

//! 提示（文字+图片）
- (void)showImageTip:(NSString *)tip image:(NSString *)imageName;

//! 提示（文字+副标题+图片）
- (void)showImageTip:(NSString *)tip message:(NSString *)message image:(NSString *)imageName;

@end
