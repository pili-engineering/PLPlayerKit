//
//  NSObject+Auth.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/19.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "NSObject+Auth.h"
#import <Photos/Photos.h>
#import <AVFoundation/AVFoundation.h>

@implementation NSObject (Auth)


+ (void)haveAlbumAccess:(void (^)(BOOL))completeBlock{
    
    PHAuthorizationStatus authStatus = [PHPhotoLibrary authorizationStatus];
    if (PHAuthorizationStatusAuthorized == authStatus){
        if (completeBlock) {
            completeBlock(YES);
        }
    } else if(PHAuthorizationStatusRestricted == authStatus || PHAuthorizationStatusDenied == authStatus){
        
        if (completeBlock) {
            completeBlock(NO);
        }
        
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"提示" message:@"照片访问限被禁止了，请前往手机 “设置-牛播放器” 打开 “照片” 开关" preferredStyle:(UIAlertControllerStyleAlert)];
        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"取消" style:(UIAlertActionStyleCancel) handler:^(UIAlertAction * _Nonnull action) {
            
        }];
        UIAlertAction *goAction = [UIAlertAction actionWithTitle:@"立即前往" style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            NSURL *url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
            [[UIApplication sharedApplication] openURL:url];
        }];
        [alert addAction:cancelAction];
        [alert addAction:goAction];
        UIViewController *viewController = [UIApplication sharedApplication].delegate.window.rootViewController;
        while (viewController.presentedViewController) {
            viewController = viewController.presentedViewController;
        }
        [viewController presentViewController:alert animated:YES completion:nil];
    } else if (PHAuthorizationStatusNotDetermined == authStatus) {
        [PHPhotoLibrary requestAuthorization:^(PHAuthorizationStatus status) {
            dispatch_main_async_safe(^{
                if (completeBlock) {
                    completeBlock(PHAuthorizationStatusAuthorized == status);
                }
            });
        }];
    }
}



+ (void)haveCameraAccess:(void (^)(BOOL))completeBlock {
    
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (AVAuthorizationStatusAuthorized == authStatus){
        if (completeBlock) {
            completeBlock(YES);
        }
    } else if(authStatus == AVAuthorizationStatusRestricted || authStatus == AVAuthorizationStatusDenied){
        
        if (completeBlock) {
            completeBlock(NO);
        }
        
        UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"提示" message:@"相机访问权限被禁止了，请前往手机 “设置-牛播放器” 打开 “相机” 开关" preferredStyle:(UIAlertControllerStyleAlert)];
        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"取消" style:(UIAlertActionStyleCancel) handler:^(UIAlertAction * _Nonnull action) {
            
        }];
        UIAlertAction *goAction = [UIAlertAction actionWithTitle:@"立即前往" style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
            NSURL *url = [NSURL URLWithString:UIApplicationOpenSettingsURLString];
            [[UIApplication sharedApplication] openURL:url];
        }];
        [alert addAction:cancelAction];
        [alert addAction:goAction];
        UIViewController *viewController = [UIApplication sharedApplication].delegate.window.rootViewController;
        while (viewController.presentedViewController) {
            viewController = viewController.presentedViewController;
        }
        [viewController presentViewController:alert animated:YES completion:nil];
    } else if (authStatus == AVAuthorizationStatusNotDetermined) {
        [AVCaptureDevice requestAccessForMediaType:AVMediaTypeVideo completionHandler:^(BOOL granted) {
            dispatch_main_async_safe(^{
                completeBlock(granted);
            });
        }];
    }
}
@end
