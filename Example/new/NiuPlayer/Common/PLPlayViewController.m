//
//  PLPlayViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/9.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLPlayViewController.h"

@interface PLPlayViewController ()

@property (nonatomic, strong) UIVisualEffectView *effectView;

@property (nonatomic, assign) BOOL isDisapper;

@property (nonatomic, strong) UIPanGestureRecognizer *panGesture;

@end

@implementation PLPlayViewController

- (void)viewDidLoad {
    
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor blackColor];
    
    _closeButton = [UIButton buttonWithType:(UIButtonTypeSystem)];
    [_closeButton setTintColor:[UIColor whiteColor]];
    [_closeButton setImage:[UIImage imageNamed:@"close"] forState:(UIControlStateNormal)];
    [_closeButton addTarget:self action:@selector(clickCloseButton) forControlEvents:(UIControlEventTouchUpInside)];
    [_closeButton setBackgroundColor:[UIColor colorWithWhite:0 alpha:.5]];
    _closeButton.layer.cornerRadius = 22;
    
    self.thumbImageView = [[UIImageView alloc] init];
    self.thumbImageView.image = [UIImage imageNamed:@"qn_niu"];
    self.thumbImageView.clipsToBounds = YES;
    self.thumbImageView.contentMode = UIViewContentModeScaleAspectFill;
    if (self.thumbImageURL) {
        [self.thumbImageView sd_setImageWithURL:self.thumbImageURL placeholderImage:self.thumbImageView.image];
    }
    if (self.thumbImage) {
        self.thumbImageView.image = self.thumbImage;
    }
    
    [self.view addSubview:self.thumbImageView];
    [self.thumbImageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.view);
    }];
    
    self.playButton = [[UIButton alloc] init];
    self.playButton.hidden = YES;
    [self.playButton addTarget:self action:@selector(clickPlayButton:) forControlEvents:(UIControlEventTouchUpInside)];
    [self.playButton setImage:[UIImage imageNamed:@"play"] forState:(UIControlStateNormal)];
    [self.view addSubview:self.playButton];
    [self.playButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.size.equalTo(CGSizeMake(60, 60));
        make.center.equalTo(self.view);
    }];
    
    [self.view addSubview:_closeButton];
    
    [_closeButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.bottom.right.equalTo(self.view).offset(-10);
        make.size.equalTo(CGSizeMake(44, 44));
    }];
    
    UIVisualEffect *effect = [UIBlurEffect effectWithStyle:UIBlurEffectStyleLight];
    self.effectView = [[UIVisualEffectView alloc] initWithEffect:effect];
    [self.thumbImageView addSubview:_effectView];
    [self.effectView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.thumbImageView);
    }];
    
    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(singleTapAction:)];
    [self.view addGestureRecognizer:singleTap ];
    
    [self setupPlayer];
    
    self.enableGesture = YES;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)setThumbImage:(UIImage *)thumbImage {
    _thumbImage = thumbImage;
    self.thumbImageView.image = thumbImage;
}

- (void)setThumbImageURL:(NSURL *)thumbImageURL {
    _thumbImageURL = thumbImageURL;
    [self.thumbImageView sd_setImageWithURL:thumbImageURL placeholderImage:self.thumbImageView.image];
}

- (void)setUrl:(NSURL *)url {
    if ([_url.absoluteString isEqualToString:url.absoluteString]) return;
    _url = url;
    
    if (self.player) {
        [self stop];
        [self setupPlayer];
        [self.player play];
    }
}

- (void) setupPlayer {
    
    NSLog(@"播放地址: %@", _url.absoluteString);
    
    PLPlayerOption *option = [PLPlayerOption defaultOption];
    PLPlayFormat format = kPLPLAY_FORMAT_UnKnown;
    NSString *urlString = _url.absoluteString.lowercaseString;
    if ([urlString hasSuffix:@"mp4"]) {
        format = kPLPLAY_FORMAT_MP4;
    } else if ([urlString hasPrefix:@"rtmp:"]) {
        format = kPLPLAY_FORMAT_FLV;
    } else if ([urlString hasSuffix:@".mp3"]) {
        format = kPLPLAY_FORMAT_MP3;
    } else if ([urlString hasSuffix:@".m3u8"]) {
        format = kPLPLAY_FORMAT_M3U8;
    }
    [option setOptionValue:@(format) forKey:PLPlayerOptionKeyVideoPreferFormat];
    [option setOptionValue:@(kPLLogNone) forKey:PLPlayerOptionKeyLogLevel];
    
    self.player = [PLPlayer playerWithURL:_url option:option];
    [self.view insertSubview:self.player.playerView atIndex:0];
    [self.player.playerView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.view);
    }];
    self.player.delegateQueue = dispatch_get_main_queue();
    self.player.playerView.contentMode = UIViewContentModeScaleAspectFit;
    self.player.delegate = self;
    self.player.loopPlay = YES;
}

- (void)clickCloseButton {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)viewDidDisappear:(BOOL)animated {
    self.isDisapper = YES;
    [self stop];
    [super viewDidDisappear:animated];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    self.isDisapper = NO;
    if (![self.player isPlaying]) {
        [self.player play];
    }
}

- (void)singleTapAction:(UIGestureRecognizer *)gesture {
    if ([self.player isPlaying]) {
        [self.player pause];
    } else {
        [self.player resume];
    }
}

- (void)clickPlayButton:(UIButton *)button {
    [self.player resume];
}

- (void)stop {
    [self.player stop];
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

- (void)showWaiting {
    [self.playButton hide];
    [self.view showFullLoading];
    [self.view bringSubviewToFront:self.closeButton];
}

- (void)hideWaiting {
    [self.view hideFullLoading];
    if (PLPlayerStatusPlaying != self.player.status) {
        [self.playButton show];
    }
}

- (void)setEnableGesture:(BOOL)enableGesture {
    if (_enableGesture == enableGesture) return;
    _enableGesture = enableGesture;
    
    if (nil == self.panGesture) {
        self.panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGesture:)];
    }
    if (enableGesture) {
        if (![[self.view gestureRecognizers] containsObject:self.panGesture]) {
            [self.view addGestureRecognizer:self.panGesture];
        }
    } else {
        [self.view removeGestureRecognizer:self.panGesture];
    }
}

- (void)panGesture:(UIPanGestureRecognizer *)panGesture {
    
    if (UIGestureRecognizerStateChanged == panGesture.state) {
        CGPoint location  = [panGesture locationInView:panGesture.view];
        CGPoint translation = [panGesture translationInView:panGesture.view];
        [panGesture setTranslation:CGPointZero inView:panGesture.view];
        
#define FULL_VALUE 200.0f
        CGFloat percent = translation.y / FULL_VALUE;
        if (location.x > self.view.bounds.size.width / 2) {// 调节音量
            
            CGFloat volume = [self.player getVolume];
            volume -= percent;
            if (volume < 0.01) {
                volume = 0.01;
            } else if (volume > 3) {
                volume = 3;
            }
            [self.player setVolume:volume];
        } else {// 调节亮度f
            CGFloat currentBrightness = [[UIScreen mainScreen] brightness];
            currentBrightness -= percent;
            if (currentBrightness < 0.1) {
                currentBrightness = 0.1;
            } else if (currentBrightness > 1) {
                currentBrightness = 1;
            }
            [[UIScreen mainScreen] setBrightness:currentBrightness];
        }
    }
}

#pragma mark - PLPlayerDelegate

- (void)playerWillBeginBackgroundTask:(PLPlayer *)player {
}

- (void)playerWillEndBackgroundTask:(PLPlayer *)player {
}

- (void)player:(PLPlayer *)player statusDidChange:(PLPlayerStatus)state
{
    if (self.isDisapper) {
        [self stop];
        [self hideWaiting];
        return;
    }
    
    if (state == PLPlayerStatusPlaying ||
        state == PLPlayerStatusPaused ||
        state == PLPlayerStatusStopped ||
        state == PLPlayerStatusError ||
        state == PLPlayerStatusUnknow ||
        state == PLPlayerStatusCompleted) {
        [self hideWaiting];
    } else if (state == PLPlayerStatusPreparing ||
               state == PLPlayerStatusReady ||
               state == PLPlayerStatusCaching) {
        [self showWaiting];
    } else if (state == PLPlayerStateAutoReconnecting) {
        [self showWaiting];
    }
}

- (void)player:(PLPlayer *)player stoppedWithError:(NSError *)error
{
    [self hideWaiting];
    NSString *info = error.userInfo[@"NSLocalizedDescription"];
    [self.view showTip:info];
}

- (void)player:(nonnull PLPlayer *)player willRenderFrame:(nullable CVPixelBufferRef)frame pts:(int64_t)pts sarNumerator:(int)sarNumerator sarDenominator:(int)sarDenominator {
    dispatch_main_async_safe(^{
        if (![UIApplication sharedApplication].isIdleTimerDisabled) {
            [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
        }
    });
    
}

- (AudioBufferList *)player:(PLPlayer *)player willAudioRenderBuffer:(AudioBufferList *)audioBufferList asbd:(AudioStreamBasicDescription)audioStreamDescription pts:(int64_t)pts sampleFormat:(PLPlayerAVSampleFormat)sampleFormat{
    return audioBufferList;
}

- (void)player:(nonnull PLPlayer *)player firstRender:(PLPlayerFirstRenderType)firstRenderType {
    if (PLPlayerFirstRenderTypeVideo == firstRenderType) {
        self.thumbImageView.hidden = YES;
    }
}

- (void)player:(nonnull PLPlayer *)player SEIData:(nullable NSData *)SEIData {
    
}

- (void)player:(PLPlayer *)player codecError:(NSError *)error {
    
    NSString *info = error.userInfo[@"NSLocalizedDescription"];
    [self.view showTip:info];
    
    [self hideWaiting];
}

- (void)player:(PLPlayer *)player loadedTimeRange:(CMTimeRange)timeRange {}

@end
