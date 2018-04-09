//
//  PLBaseViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLBaseViewController.h"

@interface PLBaseViewController ()
{
    UIButton *_reloadButton;
}

@end

@implementation PLBaseViewController

- (void)dealloc {
    printf("\n [dealloc] %s, %p\n\n", [NSStringFromClass(self.class) UTF8String], self);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    
    _reloadButton = [[UIButton alloc] init];
    [self.reloadButton setTitle:@"重新加载" forState:(UIControlStateNormal)];
    [self.view addSubview:self.reloadButton];
    self.reloadButton.layer.cornerRadius = 4;
    self.reloadButton.layer.borderWidth  = 1;
    self.reloadButton.layer.borderColor  = [UIColor colorWithWhite:.6 alpha:1].CGColor;
    [self.reloadButton setTitleColor:[UIColor colorWithWhite:.6 alpha:1] forState:(UIControlStateNormal)];
    [self.reloadButton setTitleColor:[UIColor colorWithWhite:.3 alpha:1] forState:(UIControlStateHighlighted)];
    self.reloadButton.titleLabel.font = [UIFont systemFontOfSize:14];
    [self.reloadButton addTarget:self action:@selector(clickReloadButton) forControlEvents:(UIControlEventTouchUpInside)];
    
    [self.reloadButton sizeToFit];
    
    [self.reloadButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.center.equalTo(self.view);
        make.size.equalTo(CGSizeMake(120, 30));
    }];
    
    self.reloadButton.hidden = YES;

    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)showReloadButton {
    [self.view bringSubviewToFront:self.reloadButton];
    self.reloadButton.hidden = NO;
}

- (void)hideReloadButton {
    self.reloadButton.hidden = YES;
}

- (void)clickReloadButton {
    
}

@end
