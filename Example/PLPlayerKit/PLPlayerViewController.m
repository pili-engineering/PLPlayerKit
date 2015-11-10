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

- (void)viewDidLoad {
    [super viewDidLoad];
    
    PLPlayer *player = [PLPlayer playerWithURL:self.url];
    player.delegate = self;
    self.player = player;
    
    __weak typeof(self) wself = self;
    [self.player prepareToPlayWithCompletion:^(NSError *error) {
        if (!error) {
            __strong typeof(wself) strongSelf = wself;
            UIView *playerView = strongSelf.player.playerView;
            UITapGestureRecognizer *tap = [[UITapGestureRecognizer alloc] initWithTarget:strongSelf action:@selector(tap:)];
            [playerView addGestureRecognizer:tap];
            
            [strongSelf.view addSubview:playerView];
            
            if (strongSelf.isViewLoaded && PLPlayerStatusReady == strongSelf.player.status) {
                [strongSelf.player play];
            }
        }
    }];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    if (PLPlayerStatusReady == self.player.status) {
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

- (void)tap:(UITapGestureRecognizer *)tap {
    self.player.isPlaying ? [self.player pause] : [self.player resume];
}

#pragma mark - <PLPlayerDelegate>

- (void)player:(nonnull PLPlayer *)player statusDidChange:(PLPlayerStatus)state {
    NSLog(@"%@", status[state]);
}

- (void)player:(nonnull PLPlayer *)player stoppedWithError:(nullable NSError *)error {
    NSLog(@"%@", error);
}

@end
