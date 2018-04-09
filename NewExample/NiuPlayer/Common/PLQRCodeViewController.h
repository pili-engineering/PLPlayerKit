//
//  PLQRCodeViewController.h
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/9.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLBaseViewController.h"

@class PLQRCodeViewController;
@protocol PLCodeViewControllerDelegate <NSObject>

- (void)codeViewController:(PLQRCodeViewController *)codeController scanResult:(NSString *)result;

@end

@interface PLQRCodeViewController : PLBaseViewController

@property (nonatomic, weak) id<PLCodeViewControllerDelegate> delegate;

@end
