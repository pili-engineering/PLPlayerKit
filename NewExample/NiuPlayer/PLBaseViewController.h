//
//  PLBaseViewController.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface PLBaseViewController : UIViewController

@property (nonatomic, readonly, strong) UIButton *reloadButton;

- (void)showReloadButton;

- (void)hideReloadButton;

- (void)clickReloadButton;

@end
