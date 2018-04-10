//
//  PLLiveTableViewCell.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/9.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLLiveTableViewCell.h"

@interface PLLiveTableViewCell ()

@property (nonatomic, strong) UIImageView *headerImageView;

@property (nonatomic, strong) UILabel *nameLabel;
@property (nonatomic, strong) UILabel *detailDescLabel;

@property (nonatomic, strong) UIImageView *thumbImageView;
@end


@implementation PLLiveTableViewCell

- (instancetype)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier {
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        
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
        
        self.thumbImageView = [[UIImageView alloc] init];
        self.thumbImageView.contentMode = UIViewContentModeScaleAspectFill;
        self.thumbImageView.clipsToBounds = YES;
        
        UIView *superView = self.contentView;
        
        [superView addSubview:self.headerImageView];
        [superView addSubview:self.nameLabel];
        [superView addSubview:self.thumbImageView];
        [superView addSubview:self.detailDescLabel];
        
        [self.thumbImageView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.top.equalTo(superView);
            make.height.equalTo(self.thumbImageView.mas_width).multipliedBy(1);
        }];
        
        [self.headerImageView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.top.equalTo(self.thumbImageView.mas_bottom).offset(5);
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
    [self.thumbImageView sd_setImageWithURL:[NSURL URLWithString:media.thumbURL]];
}
@end
