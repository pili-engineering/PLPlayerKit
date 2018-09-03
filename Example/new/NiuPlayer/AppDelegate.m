//
//  AppDelegate.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "AppDelegate.h"
#import "PLRootTabBarViewController.h"
#import "PLShortVideoViewController.h"
#import "PLLongVideoViewController.h"
#import "PLLiveViewController.h"

@interface AppDelegate ()

@end

@implementation AppDelegate


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    [Bugsnag startBugsnagWithApiKey:@"8c6df88d969a01d91ca4dde583be6fa1"];
    
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
    [[AVAudioSession sharedInstance] setActive:YES error:nil];
    
    self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    self.window.rootViewController = [[PLRootTabBarViewController alloc] init];
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    self.window.tintColor = [UIColor blackColor];
    
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
    PLRootTabBarViewController *rootVC = (PLRootTabBarViewController*)self.window.rootViewController;
    NSInteger index = rootVC.selectedIndex;
    if(rootVC.selectedViewController){
        switch (index) {
            case 0:{
                PLShortVideoViewController *shortVC = (PLShortVideoViewController *)rootVC.selectedViewController;
                [shortVC onUIApplication:NO];
            }
                break;
            case 1:{
                UINavigationController *navVC = rootVC.selectedViewController;
                PLLongVideoViewController *longVideoVC = (PLLongVideoViewController *)navVC.topViewController;
                [longVideoVC onUIApplication:NO];
            }
                break;
            case 2:{
                UINavigationController *navVC = rootVC.selectedViewController;
                PLLiveViewController *liveVideoVC = (PLLiveViewController *)navVC.topViewController;
                if (liveVideoVC.presentedViewController) {
                    PLPlayViewController *playerVC = (PLPlayViewController *)liveVideoVC.presentedViewController;
                    playerVC.player.enableRender = NO;
                }
            }
                break;
                
            default:
                break;
        }
    }
    NSLog(@"------ applicationWillResignActive ------");
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    PLRootTabBarViewController *rootVC = (PLRootTabBarViewController*)self.window.rootViewController;
    NSInteger index = rootVC.selectedIndex;
    if(rootVC.selectedViewController){
        switch (index) {
            case 0:{
                PLShortVideoViewController *shortVC = (PLShortVideoViewController *)rootVC.selectedViewController;
                [shortVC onUIApplication:YES];
            }
                break;
            case 1:{
                UINavigationController *navVC = rootVC.selectedViewController;
                PLLongVideoViewController *longVideoVC = (PLLongVideoViewController *)navVC.topViewController;
                [longVideoVC onUIApplication:YES];
            }
                break;
            case 2:{
                UINavigationController *navVC = rootVC.selectedViewController;
                PLLiveViewController *liveVideoVC = (PLLiveViewController *)navVC.topViewController;
                if (liveVideoVC.presentedViewController) {
                    PLPlayViewController *playerVC = (PLPlayViewController *)liveVideoVC.presentedViewController;
                    playerVC.player.enableRender = YES;
                }
            }
                break;
                
            default:
                break;
        }
    }
    NSLog(@"------ applicationDidBecomeActive ------");
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
