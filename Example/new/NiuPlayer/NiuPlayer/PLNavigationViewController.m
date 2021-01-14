//
//  PLNavigationViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/7.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLNavigationViewController.h"

@interface PLNavigationViewController ()

@end

@implementation PLNavigationViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (UIViewController *)childViewControllerForStatusBarHidden {
    return self.topViewController;
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
