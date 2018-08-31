//
//  AppDelegate.m
//  PLPlayerKitDemo
//
//  Created by 0day on 15/4/27.
//  Copyright (c) 2015年 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import "AppDelegate.h"
#import "PLPlayerViewController.h"

@interface AppDelegate () <UISplitViewControllerDelegate>

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {    
    return YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    UINavigationController *nav = (UINavigationController*)self.window.rootViewController;
    if ([nav.visibleViewController isKindOfClass:[PLPlayerViewController class]]) {
        PLPlayerViewController *playerVC = (PLPlayerViewController *)nav.visibleViewController;
        [playerVC onUIApplication:NO];
    }
    NSLog(@"%@", NSStringFromSelector(_cmd));
}
- (void)applicationWillResignActive:(UIApplication *)application {
    UINavigationController *nav = (UINavigationController*)self.window.rootViewController;
    if ([nav.visibleViewController isKindOfClass:[PLPlayerViewController class]]) {
        PLPlayerViewController *playerVC = (PLPlayerViewController *)nav.visibleViewController;
        [playerVC onUIApplication:YES];
    }
    NSLog(@"%@", NSStringFromSelector(_cmd));
}

@end
