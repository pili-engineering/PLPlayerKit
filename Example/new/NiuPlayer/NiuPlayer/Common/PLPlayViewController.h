//
//  PLPlayViewController.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/9.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLBaseViewController.h"

@interface PLPlayViewController : PLBaseViewController
<
PLPlayerDelegate
>

@property (nonatomic, strong) PLPlayer      *player;
@property (nonatomic, strong) UIButton      *playButton;
@property (nonatomic, strong) UIImageView   *thumbImageView;

@property (nonatomic, strong) UIButton      *closeButton;

@property (nonatomic, strong) NSURL *url;
@property (nonatomic, strong) UIImage *thumbImage;
@property (nonatomic, strong) NSURL *thumbImageURL;

//是否启用手指滑动调节音量和亮度, default YES
@property (nonatomic, assign) BOOL enableGesture;

@end
