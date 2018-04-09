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
    
    if (!PLPLAYER_IS_PUBLISH) {
        self.navbarView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"mask"]];
        
        self.headerImageView = [[UIImageView alloc] init];
        self.headerImageView.layer.cornerRadius = 20;
        self.headerImageView.clipsToBounds = YES;
        self.headerImageView.contentMode = UIViewContentModeScaleAspectFill;
        self.headerImageView.image = [UIImage imageNamed:self.media.headerImg];
        
        self.descLabel = [[UILabel alloc] init];
        self.descLabel.textColor = [UIColor whiteColor];
        self.descLabel.font = [UIFont systemFontOfSize:12];
        self.descLabel.numberOfLines = 0;
        self.descLabel.text = self.media.detailDesc;
        
        self.nameLabel = [[UILabel alloc] init];
        self.nameLabel.textColor = [UIColor colorWithWhite:.66 alpha:1];
        self.nameLabel.font = [UIFont systemFontOfSize:12];
        self.nameLabel.numberOfLines = 0;
        self.nameLabel.text = self.media.name;
        
        [self.view insertSubview:self.navbarView belowSubview:self.thumbImageView];
        [self.navbarView addSubview:self.headerImageView];
        [self.navbarView addSubview:self.descLabel];
        [self.navbarView addSubview:self.nameLabel];
        
        [self.navbarView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.top.right.equalTo(self.view);
            if (iPhoneX) {
                make.height.equalTo(80);
            } else {
                make.height.equalTo(64);
            }
        }];
        
        [self.headerImageView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.size.equalTo(CGSizeMake(40, 40));
            make.left.equalTo(self.navbarView).offset(10);
            make.bottom.equalTo(self.navbarView).offset(-10);
        }];
        
        [self.descLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.equalTo(self.headerImageView.mas_right).offset(10);
            make.right.equalTo(self.navbarView).offset(-10);
            make.bottom.equalTo(self.headerImageView.mas_centerY);
        }];
        
        [self.nameLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.equalTo(self.descLabel);
            make.top.equalTo(self.headerImageView.mas_centerY).offset(2);
        }];
    }

    
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
