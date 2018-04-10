//
//  PLAlertUserAgreeView.m
//  NiuPlayer
//
//  Created by 冯文秀 on 2018/4/8.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLAlertUserAgreeView.h"
@interface PLAlertUserAgreeView ()
@property (nonatomic, strong) UIView *superView;
@property (nonatomic, readwrite, assign) BOOL isShow;

@end
@implementation PLAlertUserAgreeView

- (id)initWithFrame:(CGRect)frame superView:(UIView *)superView{
    self = [super initWithFrame:frame];
    if (self) {
        self.frame = CGRectMake(0, 0, CGRectGetWidth(superView.frame), CGRectGetHeight(superView.frame));
        self.superView = superView;
        self.isShow = NO;
        
        self.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0.4];
        
        self.mainView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, 280, 180)];
        self.mainView.backgroundColor = [UIColor colorWithRed:255/255.0 green:255/255.0 blue:255/255.0 alpha:0.9];
        self.mainView.center = self.center;
        self.mainView.layer.cornerRadius = 10;
        [self addSubview:_mainView];
        
        
        UILabel *titleLabel = [[UILabel alloc]initWithFrame:CGRectMake(30, 10, 220, 30)];
        titleLabel.text = @"遵守用户协议";
        titleLabel.textColor = [UIColor blackColor];
        titleLabel.textAlignment = NSTextAlignmentCenter;
        titleLabel.font = [UIFont fontWithName:@"HelveticaNeue-Bold" size:16];
        [_mainView addSubview:titleLabel];
        
        self.agreementLabel = [[UILabel alloc]initWithFrame:CGRectMake(30, 40, 220, 40)];
        self.agreementLabel.text = @"是否同意遵守该用户协议？";;
        self.agreementLabel.textColor = [UIColor blackColor];
        self.agreementLabel.textAlignment = NSTextAlignmentCenter;
        self.agreementLabel.font = [UIFont systemFontOfSize:14];
        [_mainView addSubview:self.agreementLabel];
        
        self.agreementButton = [[UIButton alloc] initWithFrame:CGRectMake(30, 80, 220, 30)];
        self.agreementButton.titleLabel.font = [UIFont systemFontOfSize:14];
        self.agreementButton.titleLabel.textAlignment = NSTextAlignmentCenter;
        [self.agreementButton setTitle:@"牛播放器用户协议" forState:UIControlStateNormal];
        [self.agreementButton setTitleColor:[UIColor colorWithRed:45/255.0 green:152/255.0 blue:212/255.0 alpha:1] forState:UIControlStateNormal];
        [_mainView addSubview:_agreementButton];
        
        self.refuseButton = [[UIButton alloc] initWithFrame:CGRectMake(0, 120, 140, 60)];
        self.refuseButton.titleLabel.font = [UIFont fontWithName:@"HelveticaNeue-Bold" size:16];
        self.refuseButton.titleLabel.textAlignment = NSTextAlignmentCenter;
        [self.refuseButton setTitle:@"拒绝" forState:UIControlStateNormal];
        [self.refuseButton setTitleColor:[UIColor redColor] forState:UIControlStateNormal];
        [_mainView addSubview:_refuseButton];
        
        self.agreeButton = [[UIButton alloc] initWithFrame:CGRectMake(140, 120, 140, 60)];
        self.agreeButton.titleLabel.font = [UIFont systemFontOfSize:16];
        self.agreeButton.titleLabel.textAlignment = NSTextAlignmentCenter;
        [self.agreeButton setTitle:@"同意" forState:UIControlStateNormal];
        [self.agreeButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
        [_mainView addSubview:_agreeButton];
        
        UIView *horizontalLineView = [[UIView alloc]initWithFrame:CGRectMake(0, 119.5, 280, 0.5)];
        horizontalLineView.backgroundColor = [UIColor grayColor];
        [_mainView addSubview:horizontalLineView];
        
        UIView *verticaLineView = [[UIView alloc]initWithFrame:CGRectMake(140, 120, 0.5, 60)];
        verticaLineView.backgroundColor = [UIColor grayColor];
        [_mainView addSubview:verticaLineView];
    }
    return self;
}

- (void)showAlertAgreeView {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.isShow = YES;
        [self.superView insertSubview:self aboveSubview:self.superView.subviews.lastObject];
    });
}

- (void)hideAlertAgreeView {
    dispatch_async(dispatch_get_main_queue(), ^{
        self.isShow = NO;
        [self removeFromSuperview];
    });
}


/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
