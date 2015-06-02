//
//  VideoPlayerViewController.h
//  PLPlayerKitDemo
//
//  Created by 0day on 15/5/7.
//  Copyright (c) 2015年 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface VideoPlayerViewController : UIViewController

@property (nonatomic, copy) NSURL   *url;
@property (nonatomic, copy) NSDictionary    *parameters;
@property (weak, nonatomic) IBOutlet UIButton *actionButton;

- (instancetype)initWithURL:(NSURL *)url parameters:(NSDictionary *)parameters;

@end
