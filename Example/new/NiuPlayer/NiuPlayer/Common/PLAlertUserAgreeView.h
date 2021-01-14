//
//  PLAlertUserAgreeView.h
//  NiuPlayer
//
//  Created by 冯文秀 on 2018/4/8.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PLAlertUserAgreeView : UIView
@property (nonatomic, strong) UIView *mainView;
@property (nonatomic, strong) UILabel *agreementLabel;
@property (nonatomic, strong) UIButton *agreementButton;
@property (nonatomic, strong) UIButton *refuseButton;
@property (nonatomic, strong) UIButton *agreeButton;
@property (nonatomic, readonly, assign) BOOL isShow;


- (id)initWithFrame:(CGRect)frame superView:(UIView *)superView;
- (void)showAlertAgreeView;

- (void)hideAlertAgreeView;
@end
