//
//  PLScanViewController.h
//  PLPlayerKitDemo
//
//  Created by liang on 6/21/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol PLScanViewControlerDelegate <NSObject>

- (void)scanQRResult:(NSString *)qrString;

@end

@interface PLScanViewController : UIViewController

@property (nonatomic, weak) id<PLScanViewControlerDelegate> delegate;

@end
