//
//  PLShortVideoViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLShortVideoViewController.h"
#import "PLShortPlayerViewController.h"
#import "UIAlertView+BlocksKit.h"

#import "PLAgreementViewController.h"
#import "PLAlertUserAgreeView.h"

@interface PLShortVideoViewController ()

@property (nonatomic, strong) NSArray *mediaArray;
@property (nonatomic, assign) NSInteger index;
@property (nonatomic, strong) PLBaseViewController *emptyController;

@end

@implementation PLShortVideoViewController

- (void)viewDidLoad {
    
    [super viewDidLoad];
    
    if ([PLPLAYER_BUNDLEID isEqual:PLPLAYER_ENTERPRISE_BUNDLEID]) {
        // 企业版
        [self requestUpgradeURLWithCompleted:^(NSError *error, NSDictionary *upgradeDic) {
            if ([[upgradeDic objectForKey:@"Version"] integerValue] > PLPLAYER_UPGRADE) {
                [self showAlertWithMessage:@"有新版本更新" completion:^{
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:upgradeDic[@"DownloadURL"]]];
                    });
                }];
            }
        }];
    }
    
    self.mediaArray = [[NSMutableArray alloc] init];
    
    for (UIView *subView in self.view.subviews ) {
        if ([subView isKindOfClass:[UIScrollView class]]) {
            UIScrollView* scrollView = (UIScrollView*)subView;
            scrollView.delaysContentTouches = NO;
        }
    }
    self.delegate       = self;
    self.dataSource     = self;
    
    self.emptyController = [[PLBaseViewController alloc] init];
    
    [self getPlayList];
}

- (void)requestUpgradeURLWithCompleted:(void (^)(NSError *error, NSDictionary *upgradeDic))handler
{
    NSURL *url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/v1/upgrade/app?appId=%@", PLPLAYER_URL_DOMAIN, PLPLAYER_ENTERPRISE_BUNDLEID]];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
    request.HTTPMethod = @"GET";
    request.timeoutInterval = 10;
    
    NSURLSessionDataTask *task = [[NSURLSession sharedSession] dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        if (error) {
            dispatch_async(dispatch_get_main_queue(), ^{
                handler(error, nil);
            });
            return;
        }
        NSDictionary *upgradeDic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableLeaves error:&error];
        dispatch_async(dispatch_get_main_queue(), ^{
            handler(nil, upgradeDic);
        });
    }];
    [task resume];
}

- (void)showAlertWithMessage:(NSString *)message completion:(void (^)(void))completion
{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0) {
        UIAlertView *alertView = [UIAlertView bk_showAlertViewWithTitle:@"版本更新" message:message cancelButtonTitle:@"更新" otherButtonTitles:@[@"取消"] handler:^(UIAlertView *alertView, NSInteger buttonIndex) {
            if (buttonIndex == 0) {
                if (completion) {
                    completion();
                }
            }
        }];
        [alertView show];
    }
    else {
        UIAlertController *controller = [UIAlertController alertControllerWithTitle:@"版本更新" message:message preferredStyle:UIAlertControllerStyleAlert];
        [controller addAction:[UIAlertAction actionWithTitle:@"更新" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            if (completion) {
                completion();
            }
        }]];
        [controller addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
        }]];
        [self presentViewController:controller animated:YES completion:nil];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (void)clickReloadButton {
    [self.emptyController hideReloadButton];
    [self getPlayList];
}

- (void)getPlayList {
    __weak typeof(self) wself = self;
    [self.view showLoadingHUD];
    [PLHttpSession requestShortMediaList:^(NSArray *list, NSError *error) {
        [wself.view hideLoadingHUD];
        
        self.mediaArray = list;
        
        if ([[NSUserDefaults standardUserDefaults]boolForKey:@"PLNiuPlayerEnabled"]) {
            [wself reloadController];
        } 
        
        if (0 == wself.mediaArray.count && error) {
            [wself.view showTip:[NSString stringWithFormat:@"%@", error.description]];
        }
    }];
}

- (void)reloadController {
    
    if (self.mediaArray.count) {
        PLShortPlayerViewController* playerController = [[PLShortPlayerViewController alloc] init];
        if (self.index < self.mediaArray.count) {
            playerController.media = [self.mediaArray objectAtIndex:self.index];
        } else {
            playerController.media = [self.mediaArray firstObject];
            self.index = 0;
        }
        
        [self setViewControllers:@[playerController] direction:(UIPageViewControllerNavigationDirectionForward) animated:NO completion:^(BOOL finished) {
        }];
    } else {
        __weak typeof(self) wself = self;
        [self setViewControllers:@[self.emptyController] direction:(UIPageViewControllerNavigationDirectionForward) animated:NO completion:^(BOOL finished) {
            [wself.emptyController.reloadButton addTarget:wself action:@selector(clickReloadButton) forControlEvents:(UIControlEventTouchUpInside)];
            wself.emptyController.reloadButton.hidden = NO;
        }];
    }
}

- (nullable UIViewController *)pageViewController:(UIPageViewController *)pageViewController viewControllerBeforeViewController:(UIViewController *)viewController{

    if (![viewController isKindOfClass:[PLShortPlayerViewController class]]) return nil;
    
    NSInteger index = [self.mediaArray indexOfObject:[(PLShortPlayerViewController*)viewController media]];
    if (NSNotFound == index) return nil;

    index --;
    if (index < 0) return nil;
    
    PLShortPlayerViewController* playerController = [[PLShortPlayerViewController alloc] init];
    playerController.media = [self.mediaArray objectAtIndex:index];
    self.index = index;
    
    return playerController;
}

- (nullable UIViewController *)pageViewController:(UIPageViewController *)pageViewController viewControllerAfterViewController:(UIViewController *)viewController{
    
    if (![viewController isKindOfClass:[PLShortPlayerViewController class]]) return nil;
    
    NSInteger index = [self.mediaArray indexOfObject:[(PLShortPlayerViewController*)viewController media]];
    if (NSNotFound == index) return nil;

    index ++;

    if (self.mediaArray.count > index) {
        PLShortPlayerViewController* playerController = [[PLShortPlayerViewController alloc] init];
        playerController.media = [self.mediaArray objectAtIndex:index];
        self.index = index;
        return playerController;
    }
    return nil;
}

- (void)pageViewController:(UIPageViewController *)pageViewController didFinishAnimating:(BOOL)finished previousViewControllers:(NSArray<UIViewController *> *)previousViewControllers transitionCompleted:(BOOL)completed{
    
}


@end
