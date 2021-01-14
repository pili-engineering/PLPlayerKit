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
@property (nonatomic, strong) PLShortPlayerViewController *shortPlayerVC;
@end

@implementation PLShortVideoViewController

- (void)onUIApplication:(BOOL)active {
    if (self.shortPlayerVC) {
        self.shortPlayerVC.player.enableRender = active;
    }
}

- (void)viewDidLoad {
    
    [super viewDidLoad];
    
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
        
        self.shortPlayerVC = playerController;
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
