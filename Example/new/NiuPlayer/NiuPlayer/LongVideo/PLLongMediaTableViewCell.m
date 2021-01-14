//
//  PLLongMediaTableViewCell.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLLongMediaTableViewCell.h"
#import "PLPlayerView.h"


@interface PLLongMediaTableViewCell ()
<
PLPlayerViewDelegate
>

@property (nonatomic, strong) UIImageView *headerImageView;
@property (nonatomic, strong) UILabel *nameLabel;
@property (nonatomic, strong) UILabel *detailDescLabel;

@property (nonatomic, assign) BOOL isNeedReset;
@property (nonatomic, strong) PLPlayerView *playerView;

@property (nonatomic, strong) UIView *playerBgView;

@end

@implementation PLLongMediaTableViewCell

+ (CGFloat)headerViewHeight {
    return 60;
}

- (void)dealloc {
    [self stop];
}

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        
        self.isNeedReset = YES;
        
        self.headerImageView = [[UIImageView alloc] init];
        self.headerImageView.layer.cornerRadius = 20;
        self.headerImageView.contentMode = UIViewContentModeScaleAspectFill;
        self.headerImageView.clipsToBounds = YES;
        
        self.nameLabel = [[UILabel alloc] init];
        self.nameLabel.textColor = [UIColor colorWithWhite:.66 alpha:1];
        self.nameLabel.font = [UIFont systemFontOfSize:12];
        self.nameLabel.numberOfLines = 0;
        self.nameLabel.text = self.media.endUser;
        
        self.detailDescLabel = [[UILabel alloc] init];
        self.detailDescLabel.textColor = [UIColor blackColor];
        self.detailDescLabel.font = [UIFont systemFontOfSize:12];
        self.detailDescLabel.numberOfLines = 0;
        self.detailDescLabel.text = self.media.endUser;
        
        self.playerBgView = [[UIView alloc] init];
        
        self.playerView = [[PLPlayerView alloc] initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, 200)];
        self.playerView.delegate = self;
        
        UIView *superView = self.contentView;
        
        [superView addSubview:self.headerImageView];
        [superView addSubview:self.nameLabel];
        [superView addSubview:self.playerBgView];
        [superView addSubview:self.detailDescLabel];
        [self.playerBgView addSubview:self.playerView];
        
        
        NSInteger height = [UIScreen mainScreen].bounds.size.width * (9.0 / 16.0 );
        [self.playerBgView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.top.equalTo(superView);
//            make.height.equalTo(self.playerBgView.mas_width).multipliedBy(9.0/16.0);
            make.height.equalTo(height);
        }];
        
        [self.headerImageView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.top.equalTo(self.playerBgView.mas_bottom).offset(5);
            make.left.equalTo(superView).offset(10);
            make.bottom.equalTo(superView).offset(-10);
            make.size.equalTo(CGSizeMake(40, 40));
        }];
        
        [self.detailDescLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.equalTo(self.headerImageView.mas_right).offset(10);
            make.bottom.equalTo(self.headerImageView.mas_centerY);
            make.right.equalTo(superView).offset(-10);
        }];
        
        [self.nameLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.equalTo(self.detailDescLabel);
            make.top.equalTo(self.headerImageView.mas_centerY).offset(2);
        }];
        
        [self.playerView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.edges.equalTo(self.playerBgView);
        }];        
    }
    return self;
}

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)setMedia:(PLMediaInfo *)media {
    _media = media;
    
    self.headerImageView.image = [UIImage imageNamed:media.headerImg];
    self.detailDescLabel.text = media.detailDesc;
    self.nameLabel.text = media.name;
    self.playerView.media = media;
}

- (void)prepareForReuse {
    [self stop];
    [super prepareForReuse];
}

- (void)play {
    [self.playerView play];
}

- (void)stop {
    [self.playerView stop];
}

- (void)configureVideo:(BOOL)enableRender {
    [self.playerView configureVideo:enableRender];
}

- (void)playerViewEnterFullScreen:(PLPlayerView *)playerView {
    
    UIView *superView = [UIApplication sharedApplication].delegate.window.rootViewController.view;
    [self.playerView removeFromSuperview];
    [superView addSubview:self.playerView];
    [self.playerView mas_remakeConstraints:^(MASConstraintMaker *make) {
        make.width.equalTo(superView.mas_height);
        make.height.equalTo(superView.mas_width);
        make.center.equalTo(superView);
    }];
    
    [superView setNeedsUpdateConstraints];
    [superView updateConstraintsIfNeeded];
    
    [UIView animateWithDuration:.3 animations:^{
        [superView layoutIfNeeded];
    }];
    
    [self.delegate tableViewCellEnterFullScreen:self];
}

- (void)playerViewExitFullScreen:(PLPlayerView *)playerView {
    
    [self.playerView removeFromSuperview];
    [self.playerBgView addSubview:self.playerView];
    
    [self.playerView mas_remakeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.playerBgView);
    }];
    
    [self setNeedsUpdateConstraints];
    [self updateConstraintsIfNeeded];
    
    [UIView animateWithDuration:.3 animations:^{
        [self layoutIfNeeded];
    }];
    
    [self.delegate tableViewCellExitFullScreen:self];
}

- (void)playerViewWillPlay:(PLPlayerView *)playerView {
    [self.delegate tableViewWillPlay:self];
}

@end
