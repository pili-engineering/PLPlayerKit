//
//  AudioPlayerViewController.h
//  PLPlayerKit
//
//  Created by 0day on 15/8/16.
//  Copyright (c) 2015年 0dayZh. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface AudioPlayerViewController : UIViewController

@property (nonatomic, copy) NSURL   *url;
@property (nonatomic, copy) NSDictionary    *parameters;

- (instancetype)initWithURL:(NSURL *)url parameters:(NSDictionary *)parameters;

@end
