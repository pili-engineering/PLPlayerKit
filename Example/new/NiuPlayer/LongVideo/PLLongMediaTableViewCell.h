//
//  PLLongMediaTableViewCell.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import <UIKit/UIKit.h>


@class PLLongMediaTableViewCell;
@protocol PLLongMediaTableViewCellDelegate <NSObject>

- (void)tableViewWillPlay:(PLLongMediaTableViewCell *)cell;

- (void)tableViewCellEnterFullScreen:(PLLongMediaTableViewCell *)cell;

- (void)tableViewCellExitFullScreen:(PLLongMediaTableViewCell *)cell;

@end

@interface PLLongMediaTableViewCell : UITableViewCell

@property (nonatomic, weak) id<PLLongMediaTableViewCellDelegate> delegate;

@property (nonatomic, strong) PLMediaInfo *media;

- (void)play;

- (void)stop;

- (void)configureVideo:(BOOL)enableRender;


+ (CGFloat)headerViewHeight;

@end
