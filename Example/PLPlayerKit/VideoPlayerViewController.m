//
//  VideoPlayerViewController.m
//  PLPlayerKitDemo
//
//  Created by 0day on 15/5/7.
//  Copyright (c) 2015年 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import "VideoPlayerViewController.h"
#import <PLPlayerKit/PLPlayerKit.h>

@interface VideoPlayerViewController ()
<
PLVideoPlayerControllerDelegate
>

@property (nonatomic, strong) PLVideoPlayerController   *videoPlayerController;

@end

@implementation VideoPlayerViewController

- (instancetype)initWithURL:(NSURL *)url parameters:(NSDictionary *)parameters {
    self = [[UIStoryboard storyboardWithName:@"Main" bundle:nil] instantiateViewControllerWithIdentifier:@"videoPlayerVC"];
    if (self) {
        self.url = url;
        self.parameters = parameters;
    }
    
    return self;
}

- (void)dealloc {
    self.videoPlayerController = nil;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.videoPlayerController = [PLVideoPlayerController videoPlayerControllerWithContentURL:self.url
                                                                                   parameters:self.parameters];
    self.videoPlayerController.delegate = self;
    self.videoPlayerController.playerView.frame = CGRectMake(0, 0, 200, 300);
    self.videoPlayerController.playerView.center = CGPointMake(CGRectGetMidX(self.view.bounds), CGRectGetMidY(self.view.bounds));
    [self.view addSubview:self.videoPlayerController.playerView];
}

- (void)viewWillDisappear:(BOOL)animated {
    if (self.videoPlayerController.isPlaying) {
        [self.videoPlayerController pause];
    }
    
    [super viewWillDisappear:animated];
}

#pragma mark - <PLVideoPlayerControllerDelegate>

- (void)videoPlayerController:(PLVideoPlayerController *)playerController failureWithError:(NSError *)error {
    // 你可以在这里处理
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Failure"
                                                        message:error.localizedDescription
                                                       delegate:nil
                                              cancelButtonTitle:@"Cancel"
                                              otherButtonTitles:nil];
    [alertView show];
}

#pragma mark - Action

- (IBAction)actionButtonPressed:(id)sender {
    UIButton *button = (UIButton *)sender;
    
    button.enabled = NO;
    self.videoPlayerController.isPlaying ? [self.videoPlayerController pause] : [self.videoPlayerController play];
    button.enabled = YES;
}

@end
