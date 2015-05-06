//
//  PLVideoPlayerViewController.h
//  PLPlayerKit
//
//  Created on 14/11/13
//  Copyright (c) 2015年 Pili Engineering. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PLPlayerTypeDefines.h"

@interface PLVideoPlayerViewController : UIViewController

+ (instancetype)videoPlayerViewControllerWithContentURL:(NSURL *)url
                                             parameters:(NSDictionary *)parameters;

@property (nonatomic, readonly, getter=isPlaying) BOOL playing;

- (void)play;
- (void)pause;

@end
