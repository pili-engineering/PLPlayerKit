//
//  VideoPlayerViewController.m
//  PLPlayerKitDemo
//
//  Created by 0day on 15/5/7.
//  Copyright (c) 2015年 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import "VideoPlayerViewController.h"
#import <PLPlayerKit/PLPlayerKit.h>

static NSString *states[] = {
    @"Unknow",
    @"Preparing",
    @"Ready",
    @"Caching",
    @"Playing",
    @"Paused",
    @"Ended"
};

@interface VideoPlayerViewController ()
<
PLVideoPlayerControllerDelegate
>

@property (nonatomic, strong) PLVideoPlayerController   *videoPlayerController;
@property (nonatomic, strong) UIActivityIndicatorView   *indicatorView;
@property (nonatomic, strong) UISlider  *slider;
@property (nonatomic, strong) NSArray   *observers;

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

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.videoPlayerController = [PLVideoPlayerController videoPlayerControllerWithContentURL:self.url
                                                                                   parameters:self.parameters];
    self.videoPlayerController.delegate = self;
    self.videoPlayerController.playerView.frame = CGRectMake(0, 0, CGRectGetWidth(self.view.bounds) * 0.8, CGRectGetHeight(self.view.bounds) * 0.8);
    self.videoPlayerController.playerView.center = CGPointMake(CGRectGetMidX(self.view.bounds), CGRectGetMidY(self.view.bounds));
    [self.view addSubview:self.videoPlayerController.playerView];
    
    UISlider *slider = [[UISlider alloc] initWithFrame:(CGRect){0, CGRectGetMaxY(self.videoPlayerController.playerView.frame), CGRectGetWidth(self.view.bounds), 30}];
    [slider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
    [self.view addSubview:slider];
    self.slider = slider;
    
    self.indicatorView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhite];
    self.indicatorView.center = CGPointMake(CGRectGetMidX(self.view.bounds), CGRectGetMidY(self.view.bounds));
    [self.view addSubview:self.indicatorView];
    [self.indicatorView hidesWhenStopped];
    
    id observer1 = [[NSNotificationCenter defaultCenter] addObserverForName:PLAudioSessionCurrentHardwareOutputVolumeDidChangeNotification object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification *notif) {
        NSDictionary *userInfo = notif.userInfo;
        NSLog(@"Volume %.2f", [userInfo[PLAudioSessionCurrentHardwareOutputVolumeKey] floatValue]);
    }];
    
    id observer2 = [[NSNotificationCenter defaultCenter] addObserverForName:PLAudioSessionRouteDidChangeNotification object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification *notif) {
        NSDictionary *userInfo = notif.userInfo;
        NSLog(@"Reason %d", [userInfo[PLAudioSessionRouteChangeReasonKey] intValue]);
    }];
    
    self.observers = @[observer1, observer2];
}

- (void)dealloc {
    self.videoPlayerController = nil;
    
    [self.observers enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        [[NSNotificationCenter defaultCenter] removeObserver:obj];
    }];
    self.observers = nil;
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    if (!self.videoPlayerController.isPlaying) {
        [self.videoPlayerController play];
    }
}

- (void)viewWillDisappear:(BOOL)animated {
    if (self.videoPlayerController.isPlaying) {
        [self.videoPlayerController pause];
    }
    
    [super viewWillDisappear:animated];
}

#pragma mark - Orientation (OS version < iOS 8)

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
    UIView *playerView = self.videoPlayerController.playerView;
    playerView.frame =  CGRectMake(0, 0, CGRectGetHeight(self.view.bounds) * 0.8, CGRectGetWidth(self.view.bounds) * 0.8);
    playerView.center = CGPointMake(CGRectGetMidY(self.view.bounds), CGRectGetMidX(self.view.bounds));
}

#pragma mark - Orientation (OS version >= iOS 8)

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id<UIViewControllerTransitionCoordinator>)coordinator {
    UIView *playerView = self.videoPlayerController.playerView;
    playerView.frame =  CGRectMake(0, 0, size.width * 0.8, size.height * 0.8);
    playerView.center = CGPointMake(size.width * 0.5, size.height * 0.5);
}


- (void)sliderValueChanged:(id)sender {
    CGFloat value = ((UISlider *)sender).value;
    NSLog(@"%f, %f", value, self.videoPlayerController.duration);
    [self.videoPlayerController setMoviePosition:value * self.videoPlayerController.duration];
}

#pragma mark - <PLVideoPlayerControllerDelegate>

- (void)videoPlayerController:(PLVideoPlayerController *)controller playerStateDidChange:(PLVideoPlayerState)state {
    NSLog(@"Stream State: %@", states[state]);
    if (PLVideoPlayerStatePaused == state) {
        [self.actionButton setTitle:@"Play" forState:UIControlStateNormal];
    } else if (PLVideoPlayerStatePlaying == state) {
        [self.actionButton setTitle:@"Pause" forState:UIControlStateNormal];
    }
    switch (state) {
        case PLVideoPlayerStatePlaying:
        case PLVideoPlayerStateEnded:
        case PLVideoPlayerStatePaused:
            [self.indicatorView stopAnimating];
            break;
        default:
            [self.indicatorView startAnimating];
            break;
    }
}

- (void)videoPlayerControllerDecoderHasBeenReady:(PLVideoPlayerController *)controller {
    NSLog(@"Decoder is ready.");
}

- (void)videoPlayerController:(PLVideoPlayerController *)playerController failureWithError:(NSError *)error {
    NSLog(@"Error: %@", error);
    [self.indicatorView stopAnimating];
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Failure"
                                                        message:error.localizedDescription
                                                       delegate:nil
                                              cancelButtonTitle:@"Cancel"
                                              otherButtonTitles:nil];
    [alertView show];
}

- (void)videoPlayerController:(PLVideoPlayerController *)playerController positionDidChange:(NSTimeInterval)position {
    self.slider.value = position / self.videoPlayerController.duration;
}

#pragma mark - Action

- (IBAction)actionButtonPressed:(id)sender {
    UIButton *button = (UIButton *)sender;
    
    button.enabled = NO;
    self.videoPlayerController.isPlaying ? [self.videoPlayerController pause] : [self.videoPlayerController play];
    button.enabled = YES;
}

@end
