//
//  PLRootTabBarViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLRootTabBarViewController.h"
#import "PLShortVideoViewController.h"
#import "PLLongVideoViewController.h"
#import "PLNavigationViewController.h"
#import "PLLiveViewController.h"

#import "PLAgreementViewController.h"
#import "PLAlertUserAgreeView.h"

@interface PLRootTabBarViewController ()
<
UITabBarControllerDelegate
>
{
    CGRect _itemTitleFrames[5];
}

@property (nonatomic, strong) UIView *indexLine;
@property (nonatomic, strong) PLAlertUserAgreeView *alertUserAgreeView;

@end

@implementation PLRootTabBarViewController

- (void)viewWillAppear:(BOOL)animated {
    if (![[NSUserDefaults standardUserDefaults]boolForKey:@"PLNiuPlayerEnabled"]) {
        [self showNiuPlayerUserAgreement];
        [_alertUserAgreeView showAlertAgreeView];
    }
}

- (void)viewDidDisappear:(BOOL)animated {
    [_alertUserAgreeView hideAlertAgreeView];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    
    PLShortVideoViewController *shortController = [[PLShortVideoViewController alloc] initWithTransitionStyle:(UIPageViewControllerTransitionStyleScroll) navigationOrientation:(UIPageViewControllerNavigationOrientationVertical) options:@{UIPageViewControllerOptionInterPageSpacingKey:@(0)}];
    UITabBarItem *item = [[UITabBarItem alloc] initWithTitle:@"短视频" image:nil tag:1];
    shortController.tabBarItem = item;
        
    PLLongVideoViewController *longController = [[PLLongVideoViewController alloc] init];
    UINavigationController *longNavigationController = [[PLNavigationViewController alloc] initWithRootViewController:longController];
    item = [[UITabBarItem alloc] initWithTitle:@"长视频" image:nil tag:2];
    longNavigationController.tabBarItem = item;
    [longNavigationController.navigationBar setShadowImage:[[UIImage alloc] init]];
    
    PLLiveViewController *liveController = [[PLLiveViewController alloc] init];
    UINavigationController *liveNavigationController = [[PLNavigationViewController alloc] initWithRootViewController:liveController];
    item = [[UITabBarItem alloc] initWithTitle:@"直播" image:nil tag:3];
    liveNavigationController.tabBarItem = item;
    [liveNavigationController.navigationBar setShadowImage:[[UIImage alloc] init]];
    
    self.viewControllers = @[shortController, longNavigationController, liveNavigationController];
    
    [[UITabBarItem appearance] setTitleTextAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[UIFont boldSystemFontOfSize:16], NSFontAttributeName, [UIColor whiteColor], NSForegroundColorAttributeName, nil] forState:UIControlStateNormal];
    [[UITabBarItem appearance] setTitleTextAttributes:[NSDictionary dictionaryWithObjectsAndKeys:[UIFont boldSystemFontOfSize:16], NSFontAttributeName, [UIColor whiteColor], NSForegroundColorAttributeName, nil] forState:UIControlStateSelected];

    [[UITabBarItem appearance] setTitlePositionAdjustment:UIOffsetMake(0, -12)];
    
    UISwipeGestureRecognizer *leftSwipe = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(leftSwipeHandle:)];
    leftSwipe.direction = UISwipeGestureRecognizerDirectionLeft;
//    [self.view addGestureRecognizer:leftSwipe];
    
    UISwipeGestureRecognizer *rightSwipe = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(rightSwipeHandle:)];
    rightSwipe.direction = UISwipeGestureRecognizerDirectionRight;
//    [self.view addGestureRecognizer:rightSwipe];
    
    UIImage * image = [UIImage imageNamed:@"mask-3"];
    [self.tabBar setBackgroundImage:image];
    [self.tabBar setShadowImage:[[UIImage alloc] init]];
    
    self.indexLine = [[UIView alloc] init];
    self.indexLine.layer.cornerRadius = 1.5;
    self.indexLine.backgroundColor = [UIColor whiteColor];
    [self.tabBar addSubview:self.indexLine];
    
    self.delegate = self;

}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)debugLogSubView:(UIView*)view{
    
    NSLog(@"superView = %@,name = %@, frame = %@, tag = %ld", NSStringFromClass([view.superview class]), NSStringFromClass([view class]), NSStringFromCGRect(view.frame), (long)view.tag);
    if (view.subviews.count > 0) {
        for (UIView* subView in view.subviews) {
            [self debugLogSubView:subView];
        }
    }
}

- (void)setIndexLineWithIndex:(NSInteger)index {
    
    // 计算每个 Item 的 title 的 frame, 为了保证准确性，每次都计算一次
    NSMutableArray <UIView *> *labelArray = [[NSMutableArray alloc] init];
    for (UIView *subView in self.tabBar.subviews) {
        if ([NSStringFromClass(subView.class) isEqualToString:@"UITabBarButton"]) {
            for (UIView * subsubView in subView.subviews) {
                if ([NSStringFromClass(subsubView.class) isEqualToString:@"UITabBarButtonLabel"]) {
                    [labelArray addObject:subsubView];
                }
            }
        }
    }
    
    for (int i = 0; i < labelArray.count && i < ARRAY_SIZE(_itemTitleFrames); i ++) {
        CGRect rc = [labelArray[i] convertRect:labelArray[i].bounds toView:self.tabBar];
        _itemTitleFrames[i] = rc;
    }
    bubbleSort(_itemTitleFrames, labelArray.count);
    
    CGRect rc = _itemTitleFrames[index];
    rc.origin.y = rc.origin.y + rc.size.height + 5;
    rc.size.height = 3;
    [UIView animateWithDuration:.3 animations:^{
        self.indexLine.frame = rc;
    }];
}

- (void)viewDidLayoutSubviews {
    
    [super viewDidLayoutSubviews];
    
    if (self.tabBar.bounds.size.height > 49) {
        [self.tabBar setBackgroundImage:[UIImage imageNamed:@"mask-2"]];
    }
    [self setIndexLineWithIndex:self.selectedIndex];
}

- (BOOL)shouldAutorotate {
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskPortrait;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
    return UIInterfaceOrientationPortrait;
}

- (void)leftSwipeHandle:(UISwipeGestureRecognizer *)swipe {
    if (self.selectedIndex < self.viewControllers.count - 1) {
        self.selectedIndex ++;
    }
    NSLog(@"leftSwipeHandle++");
}

- (void)rightSwipeHandle:(UISwipeGestureRecognizer *)swipe {
    if (self.selectedIndex > 0) {
        self.selectedIndex --;
    }
    NSLog(@"rightSwipeHandle--");
}

- (BOOL)tabBarController:(UITabBarController *)tabBarController shouldSelectViewController:(UIViewController *)viewController {
    if (![[NSUserDefaults standardUserDefaults]boolForKey:@"PLNiuPlayerEnabled"]) {
        if (!_alertUserAgreeView.isShow) {
            [self.alertUserAgreeView showAlertAgreeView];
        }
        return NO;
    } else{
        NSInteger index = [self.viewControllers indexOfObject:viewController];
        [self setIndexLineWithIndex:index];
        return YES;
    }
}


void bubbleSort(CGRect *rcArray, NSInteger n){
    for(int i = 0 ; i < n-1; i ++) {
        for(int j = 0; j < n - i - 1; j ++) {
            if(rcArray[j].origin.x > rcArray[j + 1].origin.x) {
                CGRect tmp = rcArray[j];
                rcArray[j] = rcArray[j + 1];
                rcArray[j+1] = tmp;
            }
        }
    }
}

- (void)showNiuPlayerUserAgreement {
    self.alertUserAgreeView = [[PLAlertUserAgreeView alloc]initWithFrame:self.view.frame superView:self.view];
    [_alertUserAgreeView.agreementButton addTarget:self action:@selector(jumpUserAgreement:) forControlEvents:UIControlEventTouchUpInside];
    [_alertUserAgreeView.refuseButton addTarget:self action:@selector(refuseAgreement:) forControlEvents:UIControlEventTouchUpInside];
    [_alertUserAgreeView.agreeButton addTarget:self action:@selector(agreeUserAgreement:) forControlEvents:UIControlEventTouchUpInside];
}

- (void)jumpUserAgreement:(UIButton *)jump {
    PLAgreementViewController *agreementViewController = [[PLAgreementViewController alloc] init];
    [self presentViewController:agreementViewController animated:YES completion:nil];
}

- (void)refuseAgreement:(UIButton *)refuse {
    [_alertUserAgreeView hideAlertAgreeView];
    [[NSUserDefaults standardUserDefaults] setBool:NO forKey:@"PLNiuPlayerEnabled"];
}

- (void)agreeUserAgreement:(UIButton *)agree {
    [_alertUserAgreeView hideAlertAgreeView];
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"PLNiuPlayerEnabled"];
    self.selectedIndex = 0;
    PLShortVideoViewController *shortVideoViewController = (PLShortVideoViewController *)[self.viewControllers objectAtIndex:0];
    [shortVideoViewController reloadController];
}

@end
