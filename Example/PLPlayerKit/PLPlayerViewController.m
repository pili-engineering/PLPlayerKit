//
//  PLPlayerViewController.m
//  PLPlayerKitDemo
//
//  Created by 0dayZh on 15/10/19.
//  Copyright © 2015年 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import "PLPlayerViewController.h"
#import <PLPlayerKit/PLPlayer.h>

static NSString *status[] = {
    @"PLPlayerStatusUnknow",
    @"PLPlayerStatusPreparing",
    @"PLPlayerStatusReady",
    @"PLPlayerStatusCaching",
    @"PLPlayerStatusPlaying",
    @"PLPlayerStatusPaused",
    @"PLPlayerStatusStopped",
    @"PLPlayerStatusError"
};

@interface PLPlayerViewController ()
<
PLPlayerDelegate
>
@property (nonatomic, strong) PLPlayer  *player;
@property (nonatomic, weak) UIActivityIndicatorView *activityIndicatorView;

@end

@implementation PLPlayerViewController

- (instancetype)initWithURL:(NSURL *)url {
    self = [super init];
    if (self) {
        self.url = url;
    }
    
    return self;
}

- (void)dealloc {
    [self.player stop];
    self.player = nil;
}

- (void)startPlayer {
    __weak typeof(self) wself = self;
    [self.player prepareToPlayWithCompletion:^(NSError *error) {
        if (!error) {
            __strong typeof(wself) strongSelf = wself;
            
            // [optional] set time out interval in seconds
            strongSelf.player.timeoutIntervalForMediaPackets = 8;
            
            // add player view
            UIView *playerView = strongSelf.player.playerView;
            playerView.contentMode = UIViewContentModeScaleAspectFill;
            playerView.autoresizingMask = UIViewAutoresizingFlexibleBottomMargin | UIViewAutoresizingFlexibleTopMargin | UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleWidth;
            [strongSelf.view addSubview:playerView];
            
            if (strongSelf.isViewLoaded && PLPlayerStatusReady == strongSelf.player.status) {
                [strongSelf addActivityIndicatorView];
                [strongSelf.player play];
            }
        }
    }];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    PLPlayer *player = [PLPlayer playerWithURL:self.url];
    player.delegate = self;
    self.player = player;
    
    [self startPlayer];
    
    __weak typeof(self) wself = self;
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidEnterBackgroundNotification object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification *notif) {
        __strong typeof(wself) strongSelf = wself;
        if (strongSelf.player.isPlaying) {
            [strongSelf.player stop];
            if (strongSelf.player.playerView.superview) {
                [strongSelf.player.playerView removeFromSuperview];
            }
        }
    }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillEnterForegroundNotification object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification *notif) {
        __strong typeof(wself) strongSelf = wself;
        [strongSelf startPlayer];
    }];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    if (PLPlayerStatusReady == self.player.status) {
        [self addActivityIndicatorView];
        [self.player play];
    }
}

- (void)viewWillDisappear:(BOOL)animated {
    if (self.player.isPlaying) {
        [self.player stop];
    }
    
    [super viewWillDisappear:animated];
}

#pragma mark -

- (void)addActivityIndicatorView {
    if (self.activityIndicatorView) {
        return;
    }
    UIActivityIndicatorView *activityIndicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    activityIndicatorView.center = CGPointMake(CGRectGetMidX(self.view.bounds), CGRectGetMidY(self.view.bounds));
    [self.view addSubview:activityIndicatorView];
    [activityIndicatorView stopAnimating];
    
    self.activityIndicatorView = activityIndicatorView;
}

#pragma mark - <PLPlayerDelegate>

- (void)player:(nonnull PLPlayer *)player statusDidChange:(PLPlayerStatus)state {
    NSLog(@"%@", status[state]);
    if (PLPlayerStatusCaching == state) {
        [self.activityIndicatorView startAnimating];
    } else {
        [self.activityIndicatorView stopAnimating];
    }
}

- (void)player:(nonnull PLPlayer *)player stoppedWithError:(nullable NSError *)error {
    [self.activityIndicatorView stopAnimating];
    NSLog(@"%@", error);
}

@end
