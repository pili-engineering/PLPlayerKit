//
//  PLShortVideoViewController.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import <UIKit/UIKit.h>
@interface PLShortVideoViewController : UIPageViewController
<
UIPageViewControllerDelegate,
UIPageViewControllerDataSource
>

- (void)reloadController;
- (void)onUIApplication:(BOOL)active;

@end
