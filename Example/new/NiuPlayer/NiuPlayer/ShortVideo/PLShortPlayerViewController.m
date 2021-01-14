//
//  PLShortPlayerViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLShortPlayerViewController.h"

@interface PLShortPlayerViewController ()


@property (nonatomic, strong) UIImageView        *navbarView;
@property (nonatomic, strong) UIImageView   *headerImageView;
@property (nonatomic, strong) UILabel       *descLabel;
@property (nonatomic, strong) UILabel       *nameLabel;

@end

@implementation PLShortPlayerViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.closeButton removeFromSuperview];
    
    [self.thumbImageView sd_setImageWithURL:[NSURL URLWithString:self.media.thumbURL]];

    self.enableGesture = NO;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
- (void)setMedia:(PLMediaInfo *)media {
    _media = media;
    self.url = [NSURL URLWithString:media.videoURL];
    self.thumbImageURL = [NSURL URLWithString:media.thumbURL];
}

@end
