//
//  PLAgreementViewController.m
//  NiuPlayer
//
//  Created by 冯文秀 on 2018/4/8.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLAgreementViewController.h"

@interface PLAgreementViewController ()

@end

@implementation PLAgreementViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    self.title = @"用户协议";
    NSString * filePath = [[NSBundle mainBundle] pathForResource:@"user_agreement" ofType:@"html"];
    NSURL * url = [NSURL fileURLWithPath:filePath];
    NSURLRequest * urlRequest = [NSURLRequest requestWithURL:url];
    [self.userDelegateDataWeb loadRequest:urlRequest];
}
- (IBAction)finshAction:(id)sender {
    [self dismissViewControllerAnimated:YES completion:^{
    }];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
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
