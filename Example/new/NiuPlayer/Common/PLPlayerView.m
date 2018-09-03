//
//  PLPlayerView.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/7.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLPlayerView.h"
#import <AVFoundation/AVFoundation.h>
@class PLControlView;

@interface PLPlayerView ()
<
PLPlayerDelegate,
PLControlViewDelegate,
UIGestureRecognizerDelegate
>

@property (nonatomic, strong) UIView *topBarView;
@property (nonatomic, strong) UILabel *titleLabel;
@property (nonatomic, strong) UIButton *moreButton;
@property (nonatomic, strong) UIButton *exitfullScreenButton;

@property (nonatomic, strong) UIView *bottomBarView;
@property (nonatomic, strong) UISlider *slider;
@property (nonatomic, strong) UILabel *playTimeLabel;
@property (nonatomic, strong) UILabel *durationLabel;
@property (nonatomic, strong) UIProgressView *bufferingView;
@property (nonatomic, strong) UIButton *enterFullScreenButton;

// 在bottomBarView上面的播放暂停按钮，全屏的时候，显示
@property (nonatomic, strong) UIButton *playButton;
@property (nonatomic, strong) UIButton *pauseButton;

@property (nonatomic, strong) UIImageView *thumbImageView;
@property (nonatomic, assign) UIDeviceOrientation deviceOrientation;

@property (nonatomic, strong) PLPlayer *player;
@property (nonatomic, strong) PLPlayerOption *playerOption;
@property (nonatomic, assign) BOOL isNeedSetupPlayer;

@property (nonatomic, strong) NSTimer *playTimer;

// 在屏幕中间的播放和暂停按钮，全屏的时候，隐藏
@property (nonatomic, strong) UIButton *centerPlayButton;
@property (nonatomic, strong) UIButton *centerPauseButton;

@property (nonatomic, strong) UIButton *snapshotButton;

@property (nonatomic, strong) UIPanGestureRecognizer *panGesture;
@property (nonatomic, strong) UITapGestureRecognizer *tapGesture;

@property (nonatomic, strong) PLControlView *controlView;

// 很多时候调用stop之后，播放器可能还会返回请他状态，导致逻辑混乱，记录一下，只要调用了播放器的 stop 方法，就将 isStop 置为 YES 做标记
@property (nonatomic, assign) BOOL isStop;

// 当底部的 bottomBarView 因隐藏的时候，提供两个 progrssview 在最底部，随时能看到播放进度和缓冲进度
@property (nonatomic, strong) UIProgressView *bottomPlayProgreeeView;
@property (nonatomic, strong) UIProgressView *bottomBufferingProgressView;

// 适配iPhone X
@property (nonatomic, assign) CGFloat edgeSpace;

@end

@implementation PLPlayerView

-(void)dealloc {
    [self unsetupPlayer];
}

- (void)configureVideo:(BOOL)enableRender {
    self.player.enableRender = enableRender;
}

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.backgroundColor = [UIColor blackColor];
        if (CGRectEqualToRect([UIScreen mainScreen].bounds, CGRectMake(0, 0, 375, 812))) {
            // iPhone X
            self.edgeSpace = 20;
        } else {
            self.edgeSpace = 5;
        }
        
        [self initTopBar];
        [self initBottomBar];
        [self initOtherUI];
        [self doStableConstraint];
        
        self.bottomBarView.backgroundColor = [UIColor colorWithWhite:0.0 alpha:.2];
        self.topBarView.backgroundColor = [UIColor colorWithWhite:0.0 alpha:.2];
        
        self.deviceOrientation = UIDeviceOrientationUnknown;
        [self transformWithOrientation:UIDeviceOrientationPortrait];
        
        self.tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(singleTap:)];
        [self addGestureRecognizer:self.tapGesture];
        
        self.panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGesture:)];
        self.panGesture.delegate = self;

    }
    return self;
}

- (BOOL)isFullScreen {
    return UIDeviceOrientationPortrait != self.deviceOrientation;
}

- (void)initTopBar {
    self.topBarView = [[UIView alloc] init];
    
    self.titleLabel = [[UILabel alloc] init];
    self.titleLabel.font = [UIFont systemFontOfSize:18];
    self.titleLabel.textColor = [UIColor whiteColor];
    
    self.exitfullScreenButton = [[UIButton alloc] init];
    [self.exitfullScreenButton setImage:[UIImage imageNamed:@"player_back"] forState:(UIControlStateNormal)];
    [self.exitfullScreenButton addTarget:self action:@selector(clickExitFullScreenButton) forControlEvents:(UIControlEventTouchUpInside)];
    
    self.moreButton = [[UIButton alloc] init];
    [self.moreButton setImage:[UIImage imageNamed:@"more"] forState:(UIControlStateNormal)];
    [self.moreButton addTarget:self action:@selector(clickMoreButton) forControlEvents:(UIControlEventTouchUpInside)];
    
    [self.topBarView addSubview:self.titleLabel];
    [self.topBarView addSubview:self.exitfullScreenButton];
    [self.topBarView addSubview:self.moreButton];
    
    [self addSubview:self.topBarView];
}

- (void)initBottomBar {
    
    self.bottomBarView = [[UIView alloc] init];
    
    self.playTimeLabel = [[UILabel alloc] init];
    self.playTimeLabel.font = [UIFont systemFontOfSize:12];
    if (@available(iOS 9, *)) {
        self.playTimeLabel.font= [UIFont monospacedDigitSystemFontOfSize:12 weight:(UIFontWeightRegular)];
    }
    self.playTimeLabel.textColor = [UIColor whiteColor];
    self.playTimeLabel.text = @"0:00:00";
    [self.playTimeLabel sizeToFit];
    
    self.durationLabel = [[UILabel alloc] init];
    self.durationLabel.font = [UIFont systemFontOfSize:12];
    if (@available(iOS 9.0, *)) {
        self.durationLabel.font= [UIFont monospacedDigitSystemFontOfSize:12 weight:(UIFontWeightRegular)];
    }
    self.durationLabel.textColor = [UIColor whiteColor];
    self.durationLabel.text = @"0:00:00";
    [self.durationLabel sizeToFit];
    
    self.slider = [[UISlider alloc] init];
    self.slider.continuous = NO;
    [self.slider setThumbImage:[UIImage imageNamed:@"slider_thumb"] forState:(UIControlStateNormal)];
    self.slider.maximumTrackTintColor = [UIColor clearColor];
    self.slider.minimumTrackTintColor = [UIColor colorWithRed:.2 green:.2 blue:.8 alpha:1];
    [self.slider addTarget:self action:@selector(sliderValueChange) forControlEvents:(UIControlEventValueChanged)];
    
    self.bufferingView = [[UIProgressView alloc] init];
    self.bufferingView.progressTintColor = [UIColor colorWithWhite:1 alpha:1];
    self.bufferingView.trackTintColor = [UIColor colorWithWhite:1 alpha:.33];
    
    self.enterFullScreenButton = [UIButton buttonWithType:(UIButtonTypeSystem)];
    [self.enterFullScreenButton setTintColor:[UIColor whiteColor]];
    [self.enterFullScreenButton setImage:[UIImage imageNamed:@"full-screen"] forState:(UIControlStateNormal)];
    [self.enterFullScreenButton addTarget:self action:@selector(clickEnterFullScreenButton) forControlEvents:(UIControlEventTouchUpInside)];
    
    self.playButton = [UIButton buttonWithType:(UIButtonTypeSystem)];
    [self.playButton setTintColor:[UIColor whiteColor]];
    [self.playButton setImage:[UIImage imageNamed:@"player_play"] forState:(UIControlStateNormal)];
    [self.playButton addTarget:self action:@selector(clickPlayButton) forControlEvents:(UIControlEventTouchUpInside)];
    
    self.pauseButton = [UIButton buttonWithType:(UIButtonTypeSystem)];
    [self.pauseButton setTintColor:[UIColor whiteColor]];
    [self.pauseButton setImage:[UIImage imageNamed:@"player_stop"] forState:(UIControlStateNormal)];
    [self.pauseButton addTarget:self action:@selector(clickPauseButton) forControlEvents:(UIControlEventTouchUpInside)];
    
    [self addSubview:self.bottomBarView];
    [self.bottomBarView addSubview:self.playButton];
    [self.bottomBarView addSubview:self.pauseButton];
    [self.bottomBarView addSubview:self.playTimeLabel];
    [self.bottomBarView addSubview:self.durationLabel];
    [self.bottomBarView addSubview:self.bufferingView];
    [self.bottomBarView addSubview:self.slider];
    [self.bottomBarView addSubview:self.enterFullScreenButton];
}

- (void)initOtherUI {
    
    self.thumbImageView = [[UIImageView alloc] init];
    self.thumbImageView.contentMode =UIViewContentModeScaleAspectFill;
    self.clipsToBounds = YES;
    
    self.controlView = [[PLControlView alloc] initWithFrame:self.bounds];
    self.controlView.hidden = YES;
    self.controlView.delegate = self;
    
    self.centerPlayButton = [UIButton buttonWithType:(UIButtonTypeSystem)];
    [self.centerPlayButton setTintColor:[UIColor whiteColor]];
    [self.centerPlayButton setImage:[UIImage imageNamed:@"player_play"] forState:(UIControlStateNormal)];
    [self.centerPlayButton addTarget:self action:@selector(clickPlayButton) forControlEvents:(UIControlEventTouchUpInside)];
    
    self.centerPauseButton = [UIButton buttonWithType:(UIButtonTypeSystem)];
    [self.centerPauseButton setTintColor:[UIColor whiteColor]];
    [self.centerPauseButton setImage:[UIImage imageNamed:@"player_stop"] forState:(UIControlStateNormal)];
    [self.centerPauseButton addTarget:self action:@selector(clickPauseButton) forControlEvents:(UIControlEventTouchUpInside)];
    
    self.snapshotButton = [[UIButton alloc] init];
    [self.snapshotButton addTarget:self action:@selector(clickSnapshotButton) forControlEvents:(UIControlEventTouchUpInside)];
    [self.snapshotButton setImage:[UIImage imageNamed:@"screen-cut"] forState:(UIControlStateNormal)];
    
    self.bottomPlayProgreeeView = [[UIProgressView alloc] init];
    self.bottomPlayProgreeeView.progressTintColor = [UIColor colorWithRed:.2 green:.2 blue:.8 alpha:1];
    self.bottomPlayProgreeeView.trackTintColor = [UIColor clearColor];
    
    self.bottomBufferingProgressView = [[UIProgressView alloc] init];
    self.bottomBufferingProgressView.progressTintColor = [UIColor colorWithWhite:1 alpha:1];
    self.bottomBufferingProgressView.trackTintColor = [UIColor colorWithWhite:1 alpha:.33];
    
    [self insertSubview:self.thumbImageView atIndex:0];
    [self addSubview:self.snapshotButton];
    [self addSubview:self.centerPauseButton];
    [self addSubview:self.centerPlayButton];
    [self addSubview:self.controlView];
    [self addSubview:self.bottomBufferingProgressView];
    [self addSubview:self.bottomPlayProgreeeView];
    
    self.pauseButton.hidden = YES;
    self.centerPauseButton.hidden = YES;
}

// 这些控件的 Constraints 不会随着全屏和非全屏而需要做改变的
- (void)doStableConstraint {
    
    [self.topBarView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.equalTo(self);
        make.bottom.equalTo(self.mas_top);
        make.height.equalTo(44);
    }];
    
    [self.exitfullScreenButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.bottom.equalTo(self.topBarView);
        make.left.equalTo(self.topBarView).offset(self.edgeSpace);
        make.width.equalTo(self.exitfullScreenButton.mas_height);
    }];
    
    [self.moreButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.top.bottom.equalTo(self.topBarView);
        make.right.equalTo(self.topBarView).offset(-self.edgeSpace);
        make.width.equalTo(self.moreButton.mas_height);
    }];
    
    [self.titleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.equalTo(self.exitfullScreenButton.mas_right);
        make.right.equalTo(self.moreButton.mas_left);
        make.centerY.equalTo(self.topBarView);
    }];
    
    [self.bottomBarView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.equalTo(self);
        make.top.equalTo(self.mas_bottom);
        make.height.equalTo(44);
    }];
    
    [self.slider mas_makeConstraints:^(MASConstraintMaker *make) {
        make.centerY.equalTo(self.bottomBarView);
        make.left.equalTo(self.playTimeLabel.mas_right).offset(5);
        make.right.equalTo(self.durationLabel.mas_left).offset(-5);
    }];
    
    [self.durationLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.right.equalTo(self.enterFullScreenButton.mas_left);
        make.centerY.equalTo(self.bottomBarView);
        make.size.equalTo(self.durationLabel.bounds.size);
    }];
    
    [self.pauseButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.playButton);
    }];
    
    [self.centerPlayButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.center.equalTo(self);
        make.size.equalTo(CGSizeMake(64, 64));
    }];
    
    [self.centerPauseButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.centerPlayButton);
    }];
    
    [self.bufferingView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.equalTo(self.slider);
        make.centerY.equalTo(self.slider).offset(.5);
    }];
    
    [self.playTimeLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.equalTo(self.playButton.mas_right);
        make.width.equalTo(self.playTimeLabel.bounds.size.width);
        make.centerY.equalTo(self.bottomBarView);
    }];
    
    [self.thumbImageView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self);
    }];
    
    [self.snapshotButton mas_makeConstraints:^(MASConstraintMaker *make) {
        make.centerY.equalTo(self);
        make.right.equalTo(self).offset(-self.edgeSpace);
        make.size.equalTo(CGSizeMake(60, 60));
    }];
    
    [self.bottomBufferingProgressView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.left.right.bottom.equalTo(self);
        make.height.equalTo(3);
    }];
    
    [self.bottomPlayProgreeeView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.bottomBufferingProgressView);
    }];
    
    [self.controlView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.width.top.bottom.equalTo(self);
        make.left.equalTo(self).offset(290);
    }];
    self.controlView.hidden = YES;
}

- (void)addTimer {
    [self removeTimer];
    self.playTimer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(timerAction) userInfo:nil repeats:YES];
}

- (void)removeTimer {
    if (self.playTimer) {
        [self.playTimer invalidate];
        self.playTimer = nil;
    }
}

- (void)timerAction {
    self.slider.value = CMTimeGetSeconds(self.player.currentTime);
    if (CMTimeGetSeconds(self.player.totalDuration)) {
        int duration = self.slider.value + .5;
        int hour = duration / 3600;
        int min  = (duration % 3600) / 60;
        int sec  = duration % 60;
        self.playTimeLabel.text = [NSString stringWithFormat:@"%d:%02d:%02d", hour, min, sec];
        self.bottomPlayProgreeeView.progress = self.slider.value / CMTimeGetSeconds(self.player.totalDuration);
    }
}

- (void)panGesture:(UIPanGestureRecognizer *)panGesture {

    if (UIGestureRecognizerStateChanged == panGesture.state) {
        CGPoint location  = [panGesture locationInView:panGesture.view];
        CGPoint translation = [panGesture translationInView:panGesture.view];
        [panGesture setTranslation:CGPointZero inView:panGesture.view];

#define FULL_VALUE 200.0f
        CGFloat percent = translation.y / FULL_VALUE;
        if (location.x > self.bounds.size.width / 2) {// 调节音量
            
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

- (void)singleTap:(UIGestureRecognizer *)gesture {
    
    // 如果还木有初始化，直接初始化播放
    if (self.isNeedSetupPlayer || PLPlayerStatusStopped == self.player.status) {
        [self play];
        return;
    }
    
    if (PLPlayerStatusPaused == self.player.status) {
        [self resume];
        return;
    }
    
    if (PLPlayerStatusPlaying == self.player.status) {
        if (self.bottomBarView.frame.origin.y >= self.bounds.size.height) {
            [self showBar];
        } else {
            [self hideBar];
        }
    }
}

- (void)hideBar {
    
    if (PLPlayerStatusPlaying != self.player.status) return;
    
    [self hideTopBar];
    [self hideBottomBar];
    self.centerPauseButton.hidden = YES;
    [self doConstraintAnimation];
    
}

- (void)showBar {
    
    [self showBottomBar];
    self.centerPauseButton.hidden = NO;
    if ([self isFullScreen]) {
        [self showTopBar];
    }
    [self doConstraintAnimation];
    
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(hideBar) object:nil];
    [self performSelector:@selector(hideBar) withObject:nil afterDelay:3];
}

- (void)showControlView {
    
    [self hideBar];
    [self hideTopBar];
    self.centerPauseButton.hidden = YES;
    self.centerPlayButton.hidden = YES;
    
    self.controlView.hidden = NO;
    [self.controlView mas_remakeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self);
    }];
    
    [self doConstraintAnimation];
}

- (void)hideControlView {
    
    [self.controlView mas_remakeConstraints:^(MASConstraintMaker *make) {
        make.width.top.bottom.equalTo(self);
        make.left.equalTo(self).offset(290);
    }];
    
    [self setNeedsUpdateConstraints];
    [self updateConstraintsIfNeeded];
    
    [UIView animateWithDuration:.3 animations:^{
        [self layoutIfNeeded];
    } completion:^(BOOL finished) {
        self.controlView.hidden = YES;
    }];
}

- (void)clickExitFullScreenButton {
    [self transformWithOrientation:UIDeviceOrientationPortrait];
}

- (void)clickEnterFullScreenButton {
    if (UIDeviceOrientationLandscapeRight == [[UIDevice currentDevice]orientation]) {
        [self transformWithOrientation:UIDeviceOrientationLandscapeRight];
    } else {
        [self transformWithOrientation:UIDeviceOrientationLandscapeLeft];
    }
}

- (void)clickMoreButton {
    [self removeGestureRecognizer:self.tapGesture];
    [self removeGestureRecognizer:self.panGesture];
    [self showControlView];
}

- (void)clickSnapshotButton {
    
    __weak typeof(self) wself = self;

    [NSObject haveAlbumAccess:^(BOOL isAuth) {
        if (!isAuth) return;
        
        [wself.player getScreenShotWithCompletionHandler:^(UIImage * _Nullable image) {
            if (image) {
                [wself showTip:@"拍照成功"];
                UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil);
            }
        }];
    }];
}

- (void)sliderValueChange {
    [self.player seekTo:CMTimeMake(self.slider.value * 1000, 1000)];
}

- (void)doConstraintAnimation {
    [self setNeedsUpdateConstraints];
    [self updateConstraintsIfNeeded];

    [UIView animateWithDuration:.3 animations:^{
        [self layoutIfNeeded];
    }];
}

- (void)hideTopBar {
    [self.topBarView mas_remakeConstraints:^(MASConstraintMaker *make) {
        make.left.right.equalTo(self);
        make.bottom.equalTo(self.mas_top);
        make.height.equalTo(44);
    }];
}

- (void)hideBottomBar {
    [self.bottomBarView mas_remakeConstraints:^(MASConstraintMaker *make) {
        make.left.right.equalTo(self);
        make.top.equalTo(self.mas_bottom);
        make.height.equalTo(44);
    }];
    
    self.snapshotButton.hidden = YES;
    
    if (PLPlayerStatusPlaying == self.player.status ||
        PLPlayerStatusPaused == self.player.status ||
        PLPlayerStatusCaching == self.player.status) {
        [self showBottomProgressView];
    }
}

- (void)showTopBar {
    [self.topBarView mas_remakeConstraints:^(MASConstraintMaker *make) {
        make.left.top.right.equalTo(self);
        make.height.equalTo(44);
    }];
    self.snapshotButton.hidden = NO;
}

- (void)showBottomBar {
    [self.bottomBarView mas_remakeConstraints:^(MASConstraintMaker *make) {
        make.left.bottom.right.equalTo(self);
        make.height.equalTo(44);
    }];
    
    [self hideBottomProgressView];
}

- (void)showBottomProgressView {
//    self.bottomBufferingProgressView.hidden = NO;
//    self.bottomPlayProgreeeView.hidden = NO;
}

- (void)hideBottomProgressView {
    self.bottomBufferingProgressView.hidden = YES;
    self.bottomPlayProgreeeView.hidden = YES;
}

- (void)transformWithOrientation:(UIDeviceOrientation)or {
    
    if (or == self.deviceOrientation) return;
    if (!(UIDeviceOrientationPortrait == or || UIDeviceOrientationLandscapeLeft == or || UIDeviceOrientationLandscapeRight == or)) return;
    
    BOOL isFirst = UIDeviceOrientationUnknown == self.deviceOrientation;
    
    if (or == UIDeviceOrientationPortrait) {
        
        [self removeGestureRecognizer:self.panGesture];
        self.snapshotButton.hidden = YES;
        
        [self.playButton mas_remakeConstraints:^(MASConstraintMaker *make) {
            make.top.bottom.equalTo(self.bottomBarView);
            make.left.equalTo(self.bottomBarView).offset(5);
            make.width.equalTo(0);
        }];
        
        [self.enterFullScreenButton mas_remakeConstraints:^(MASConstraintMaker *make) {
            make.right.top.bottom.equalTo(self.bottomBarView);
            make.width.equalTo(self.enterFullScreenButton.mas_height);
        }];
        
        [self.centerPlayButton mas_remakeConstraints:^(MASConstraintMaker *make) {
            make.center.equalTo(self);
            make.size.equalTo(CGSizeMake(64, 64));
        }];
        
        if (!isFirst) {
            [self hideTopBar];
            [self hideControlView];
            [self doConstraintAnimation];
            [self.delegate playerViewExitFullScreen:self];
            if (![self.gestureRecognizers containsObject:self.tapGesture]) {
                [self addGestureRecognizer:self.tapGesture];
            }
        }
        [UIView animateWithDuration:.3 animations:^{
            self.transform = CGAffineTransformMakeRotation(0);
        }];
        
    } else {
        
        if (![[self gestureRecognizers] containsObject:self.panGesture]) {
            [self addGestureRecognizer:self.panGesture];
        }
        
        CGFloat duration = .5;
        if (!UIDeviceOrientationIsLandscape(self.deviceOrientation)) {
            duration = .3;
            
            [self.playButton mas_remakeConstraints:^(MASConstraintMaker *make) {
                make.top.bottom.equalTo(self.bottomBarView);
                make.left.equalTo(self.bottomBarView).offset(self.edgeSpace);
                make.width.equalTo(self.playButton.mas_height);
            }];
            
            [self.enterFullScreenButton mas_remakeConstraints:^(MASConstraintMaker *make) {
                make.top.bottom.equalTo(self.bottomBarView);
                make.right.equalTo(self.bottomBarView).offset(-self.edgeSpace);
                make.width.equalTo(0);
            }];
            
            [self.centerPlayButton mas_remakeConstraints:^(MASConstraintMaker *make) {
                make.center.equalTo(self);
                make.size.equalTo(CGSizeMake(0, 0));
            }];
            
            [self doConstraintAnimation];
        }
        
        [UIView animateWithDuration:duration animations:^{
            self.transform = UIDeviceOrientationLandscapeLeft == or ? CGAffineTransformMakeRotation(M_PI/2) : CGAffineTransformMakeRotation(3*M_PI/2);
        }];
        
        if (UIDeviceOrientationUnknown != self.deviceOrientation) {
            [self.delegate playerViewEnterFullScreen:self];
        }
    }
    
    self.deviceOrientation = or;
}

-(void)addFullStreenNotify{
    
    [self removeFullStreenNotify];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(recvDeviceOrientationChangeNotify:) name:UIDeviceOrientationDidChangeNotification object:nil];
}

-(void)removeFullStreenNotify{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIDeviceOrientationDidChangeNotification object:nil];
}

-(void)recvDeviceOrientationChangeNotify:(NSNotification*)info{
    UIDeviceOrientation or = [[UIDevice currentDevice]orientation];
    [self transformWithOrientation:or];
}

- (void)clickPauseButton {
    [self pause];
}

- (void)clickPlayButton {
    if (PLPlayerStatusPaused == self.player.status) {
        [self resume];
    } else {
        [self play];
    }
}

- (void)setupPlayer {
    [self unsetupPlayer];
    
    NSLog(@"播放地址: %@", _media.videoURL);
    self.thumbImageView.hidden = NO;
    [self.thumbImageView sd_setImageWithURL:[NSURL URLWithString:_media.thumbURL]];
    
    self.playerOption = [PLPlayerOption defaultOption];
    PLPlayFormat format = kPLPLAY_FORMAT_UnKnown;
    NSString *urlString = _media.videoURL.lowercaseString;
    if ([urlString hasSuffix:@"mp4"]) {
        format = kPLPLAY_FORMAT_MP4;
    } else if ([urlString hasPrefix:@"rtmp:"]) {
        format = kPLPLAY_FORMAT_FLV;
    } else if ([urlString hasSuffix:@".mp3"]) {
        format = kPLPLAY_FORMAT_MP3;
    } else if ([urlString hasSuffix:@".m3u8"]) {
        format = kPLPLAY_FORMAT_M3U8;
    }
    [self.playerOption setOptionValue:@(format) forKey:PLPlayerOptionKeyVideoPreferFormat];
    [self.playerOption setOptionValue:@(kPLLogNone) forKey:PLPlayerOptionKeyLogLevel];
    
    NSDate *date = [NSDate date];
    self.player = [PLPlayer playerWithURL:[NSURL URLWithString:_media.videoURL] option:self.playerOption];
    
    NSLog(@"playerWithURL 耗时： %f s",[[NSDate date] timeIntervalSinceDate:date]);
    
    self.player.delegateQueue = dispatch_get_main_queue();
    self.player.playerView.contentMode = UIViewContentModeScaleAspectFit;
    self.player.delegate = self;
    self.player.loopPlay = YES;
    
    [self insertSubview:self.player.playerView atIndex:0];
    self.player.playerView.frame = self.bounds;
    [self.player.playerView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self);
    }];
}

- (void)unsetupPlayer {
    [self stop];
    
    if (self.player.playerView.superview) {
        [self.player.playerView removeFromSuperview];
    }
    [self removeTimer];
}

- (void)setMedia:(PLMediaInfo *)media {
    _media = media;
    self.titleLabel.text = media.detailDesc;
    self.isNeedSetupPlayer = YES;
    [self.thumbImageView sd_setImageWithURL:[NSURL URLWithString:_media.thumbURL]];
}

- (void)play {
    
    if (self.isNeedSetupPlayer) {
        [self setupPlayer];
        self.isNeedSetupPlayer = NO;
    }
    self.isStop = NO;
    
    [self.delegate playerViewWillPlay:self];
    [self addFullStreenNotify];
    [self addTimer];
    [self resetButton:YES];
    
    if (!(PLPlayerStatusReady == self.player.status ||
        PLPlayerStatusOpen == self.player.status ||
        PLPlayerStatusCaching == self.player.status ||
        PLPlayerStatusPlaying == self.player.status ||
        PLPlayerStatusPreparing == self.player.status)
        ) {
        NSDate *date = [NSDate date];
        [self.player play];
        NSLog(@"play 耗时： %f s",[[NSDate date] timeIntervalSinceDate:date]);
    }
}

- (void)pause {
    [self.player pause];
    [self resetButton:NO];
}

- (void)resume {
    
    [self.delegate playerViewWillPlay:self];
    [self.player resume];
    [self resetButton:YES];
}

- (void)stop {
    
    NSDate *date = nil;
    if ([self.player isPlaying]) {
        date = [NSDate date];
    }
    [self.player stop];
    if (date) {
        NSLog(@"stop 耗时： %f s",[[NSDate date] timeIntervalSinceDate:date]);
    }
    
    
    [self removeFullStreenNotify];
    [self resetUI];
    [self.controlView resetStatus];
    self.isStop = YES;
    [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

- (void)resetUI {
    
    self.bufferingView.progress = 0;
    self.slider.value = 0;
    self.playTimeLabel.text = @"0:00:00";
    self.durationLabel.text = @"0:00:00";
    self.thumbImageView.hidden = NO;
    
    [self resetButton:NO];
    [self hideFullLoading];
    
    [self hideTopBar];
    [self hideBottomBar];
    [self hideBottomProgressView];
    [self doConstraintAnimation];
    
}

- (void)resetButton:(BOOL)isPlaying {
    
    self.playButton.hidden = isPlaying;
    self.pauseButton.hidden = !isPlaying;
    
    if (isPlaying) {
        self.centerPauseButton.hidden = NO;
        self.centerPlayButton.hidden  = YES;
    } else {
        self.centerPauseButton.hidden = YES;
        [self.centerPlayButton show];
    }
}

// 避免 pan 手势将 slider 手势给屏蔽掉
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer {
    if (gestureRecognizer == self.panGesture) {
        CGPoint point = [gestureRecognizer locationInView:self];
        return !CGRectContainsPoint(self.bottomBarView.frame, point);
    }
    return YES;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    if (gestureRecognizer == self.panGesture) {
        CGPoint point = [touch locationInView:self];
        return !CGRectContainsPoint(self.bottomBarView.frame, point);
    }
    return YES;    
}

// PLControlViewDelegate
- (void)controlViewClose:(PLControlView *)controlView {
    
    [self hideControlView];
    
    if (![[self gestureRecognizers] containsObject:self.panGesture]) {
        [self addGestureRecognizer:self.panGesture];
    }
    if (![[self gestureRecognizers] containsObject:self.tapGesture]) {
        [self addGestureRecognizer:self.tapGesture];
    }
}

- (void)controlView:(PLControlView *)controlView speedChange:(CGFloat)speed {
    [self.player setPlaySpeed:speed];
}

- (void)controlView:(PLControlView *)controlView ratioChange:(PLPlayerRatio)ratio {
    CGRect rc = CGRectMake(0, 0, self.player.width, self.player.height);
    if (PLPlayerRatioDefault == ratio) {
        [self.player setVideoClipFrame:CGRectZero];
    } else if (PLPlayerRatioFullScreen == ratio) {
        [self.player setVideoClipFrame:rc];
    } else if (PLPlayerRatio16x9 == ratio) {
        CGFloat width = 0;
        CGFloat height = 0;
        if (rc.size.width / rc.size.height > 16.0 / 9.0) {
            height = rc.size.height;
            width = rc.size.height * 16.0 / 9.0;
            rc.origin.x = (rc.size.width - width ) / 2;
        } else {
            width = rc.size.width;
            height = rc.size.width * 9.0 / 16.0;
            rc.origin.y = (rc.size.height - height ) / 2;
        }
        rc.size.width = width;
        rc.size.height = height;
        [self.player setVideoClipFrame:rc];
    } else if (PLPlayerRatio4x3 == ratio) {
        CGFloat width = 0;
        CGFloat height = 0;
        if (rc.size.width / rc.size.height > 4.0 / 3.0) {
            height = rc.size.height;
            width = rc.size.height * 4.0 / 3.0;
            rc.origin.x = (rc.size.width - width ) / 2;
        } else {
            width = rc.size.width;
            height = rc.size.width * 3.0 / 4.0;
            rc.origin.y = (rc.size.height - height ) / 2;
        }
        rc.size.width = width;
        rc.size.height = height;
        [self.player setVideoClipFrame:rc];
    }
}

- (void)controlView:(PLControlView *)controlView backgroundPlayChange:(BOOL)isBackgroundPlay {
    [self.player setBackgroundPlayEnable:isBackgroundPlay];
}

- (void)controlViewMirror:(PLControlView *)controlView {
    if (PLPlayerFlipHorizonal != self.player.rotationMode) {
        self.player.rotationMode = PLPlayerFlipHorizonal;
    } else {
        self.player.rotationMode = PLPlayerNoRotation;
    }
}

- (void)controlViewRotate:(PLControlView *)controlView {
    
    PLPlayerRotationsMode mode = self.player.rotationMode;
    mode ++;
    if (mode > PLPlayerRotate180) {
        mode = PLPlayerNoRotation;
    }
    self.player.rotationMode = mode;
}

- (BOOL)controlViewCache:(PLControlView *)controlView {
    if ([self.playerOption optionValueForKey:PLPlayerOptionKeyVideoCacheFolderPath]) {
        [_playerOption setOptionValue:nil forKey:PLPlayerOptionKeyVideoCacheFolderPath];
        [_playerOption setOptionValue:nil forKey:PLPlayerOptionKeyVideoCacheExtensionName];
        return NO;
    } else {
        NSString* docPathDir = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
        docPathDir = [docPathDir stringByAppendingString:@"/PLCache/"];
        [_playerOption setOptionValue:docPathDir forKey:PLPlayerOptionKeyVideoCacheFolderPath];
        [_playerOption setOptionValue:@"mp4" forKey:PLPlayerOptionKeyVideoCacheExtensionName];
        return YES;
    }
}


#pragma mark - PLPlayerDelegate

- (void)playerWillBeginBackgroundTask:(PLPlayer *)player {
}

- (void)playerWillEndBackgroundTask:(PLPlayer *)player {
}

- (void)player:(PLPlayer *)player statusDidChange:(PLPlayerStatus)state
{
    if (self.isStop) {
        static NSString * statesString[] = {
            @"PLPlayerStatusUnknow"
            @"PLPlayerStatusPreparing",
            @"PLPlayerStatusReady",
            @"PLPlayerStatusOpen",
            @"PLPlayerStatusCaching",
            @"PLPlayerStatusPlaying",
            @"PLPlayerStatusPaused",
            @"PLPlayerStatusStopped",
            @"PLPlayerStatusError",
            @"PLPlayerStateAutoReconnecting",
            @"PLPlayerStatusCompleted"
        };
//        NSLog(@"stop statusDidChange self,= %p state = %@", self, statesString[state]);
        [self stop];
        return;
    }
    
    if (state == PLPlayerStatusPlaying ||
        state == PLPlayerStatusPaused ||
        state == PLPlayerStatusStopped ||
        state == PLPlayerStatusError ||
        state == PLPlayerStatusUnknow ||
        state == PLPlayerStatusCompleted) {
        [self hideFullLoading];
    } else if (state == PLPlayerStatusPreparing ||
               state == PLPlayerStatusReady ||
               state == PLPlayerStatusCaching) {
        [self showFullLoading];
        self.centerPauseButton.hidden = YES;
    } else if (state == PLPlayerStateAutoReconnecting) {
        [self showFullLoading];
        self.centerPauseButton.hidden = YES;
        // alert 重新
        [self showTip:@"重新连接..."];
    }
    
    //开始播放之后，如果 bar 是显示的，则 3 秒之后自动隐藏
    if (PLPlayerStatusPlaying == state) {
        if (self.bottomBarView.frame.origin.y >= self.bounds.size.height) {
            [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(hideBar) object:nil];
            [self performSelector:@selector(hideBar) withObject:nil afterDelay:3];
        }
    }
}

- (void)player:(PLPlayer *)player stoppedWithError:(NSError *)error
{
    NSString *info = error.userInfo[@"NSLocalizedDescription"];
    [self showTip:info];
    
    [self stop];
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
    self.slider.maximumValue = CMTimeGetSeconds(self.player.totalDuration);
    self.slider.minimumValue = 0;
    
    CGFloat fduration = CMTimeGetSeconds(self.player.totalDuration);
    int duration = fduration + .5;
    int hour = duration / 3600;
    int min  = (duration % 3600) / 60;
    int sec  = duration % 60;
    self.durationLabel.text = [NSString stringWithFormat:@"%d:%02d:%02d", hour, min, sec];
}

- (void)player:(nonnull PLPlayer *)player SEIData:(nullable NSData *)SEIData {
    
}

- (void)player:(PLPlayer *)player codecError:(NSError *)error {
    NSString *info = error.userInfo[@"NSLocalizedDescription"];
    [self showTip:info];
    
    [self stop];
}

- (void)player:(PLPlayer *)player loadedTimeRange:(CMTime)timeRange {
    
    float startSeconds = 0;
    float durationSeconds = CMTimeGetSeconds(timeRange);
    CGFloat totalDuration = CMTimeGetSeconds(self.player.totalDuration);
    self.bufferingView.progress = (durationSeconds - startSeconds) / totalDuration;
    self.bottomBufferingProgressView.progress = self.bufferingView.progress;
}

@end



@implementation PLControlView

static NSString * speedString[] = {@"0.5", @"0.75", @"1.0", @"1.25", @"1.5"};

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        UIView *bgView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 300, 300)];
        bgView.backgroundColor = [UIColor colorWithWhite:0 alpha:.5];
        [self addSubview:bgView];
        
        [bgView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.right.bottom.top.equalTo(self);
            make.width.equalTo(290);
        }];
        
        UIButton *dismissButton = [UIButton buttonWithType:(UIButtonTypeSystem)];
        [dismissButton addTarget:self action:@selector(clickCloseButton) forControlEvents:(UIControlEventTouchUpInside)];
        [self addSubview:dismissButton];
        [dismissButton mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.top.bottom.equalTo(self);
            make.right.equalTo(bgView.mas_left);
        }];
        
        
        self.scrollView = [[UIScrollView alloc] init];
        UIView *contentView = [[UIView alloc] init];
        
        UIView *barView = [[UIView alloc] init];
        [barView setBackgroundColor:[UIColor colorWithWhite:0 alpha:.5]];
        
        UILabel *title = [[UILabel alloc] init];
        title.textAlignment = NSTextAlignmentCenter;
        title.font = [UIFont systemFontOfSize:18];
        title.textColor = [UIColor whiteColor];
        title.text = @"播放设置";
        
        UIButton *closeButton = [UIButton buttonWithType:(UIButtonTypeSystem)];
        [closeButton setTintColor:[UIColor whiteColor]];
        [closeButton setImage:[UIImage imageNamed:@"player_close"] forState:(UIControlStateNormal)];
        [closeButton addTarget:self action:@selector(clickCloseButton) forControlEvents:(UIControlEventTouchUpInside)];
        
        [bgView addSubview:barView];
        [bgView addSubview:self.scrollView];
        [barView addSubview:title];
        [barView addSubview:closeButton];
        [self.scrollView addSubview:contentView];
        
        [barView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.top.equalTo(bgView);
            make.height.equalTo(50);
        }];
        
        [title mas_makeConstraints:^(MASConstraintMaker *make) {
            make.edges.equalTo(barView);
        }];
        
        if (CGRectEqualToRect([UIScreen mainScreen].bounds, CGRectMake(0, 0, 375, 812))) {
            // iPhone X
            [closeButton mas_makeConstraints:^(MASConstraintMaker *make) {
                make.top.bottom.equalTo(barView);
                make.right.equalTo(barView).offset(-20);
                make.width.equalTo(closeButton.mas_height);
            }];
        } else {
            [closeButton mas_makeConstraints:^(MASConstraintMaker *make) {
                make.top.bottom.equalTo(barView);
                make.right.equalTo(barView).offset(-5);
                make.width.equalTo(closeButton.mas_height);
            }];
        }
        
        [self.scrollView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.bottom.right.equalTo(bgView);
            make.top.equalTo(barView.mas_bottom);
        }];
        
        [contentView mas_makeConstraints:^(MASConstraintMaker *make) {
            make.edges.equalTo(self.scrollView);
            make.width.equalTo(bgView);
        }];
        
        self.speedTitleLabel = [[UILabel alloc] init];
        self.speedTitleLabel.font = [UIFont systemFontOfSize:12];
        self.speedTitleLabel.textColor = [UIColor colorWithWhite:.8 alpha:1];
        self.speedTitleLabel.text = @"播放速度：";
        [self.speedTitleLabel sizeToFit];
        
        self.speedValueLabel = [[UILabel alloc] init];
        self.speedValueLabel.font = [UIFont systemFontOfSize:12];
        self.speedValueLabel.textColor = [UIColor colorWithRed:.33 green:.66 blue:1 alpha:1];
        self.speedValueLabel.text = @"1.0";
        
        NSDictionary *dic = [NSDictionary dictionaryWithObjectsAndKeys:[UIColor colorWithWhite:1 alpha:.5], NSForegroundColorAttributeName, [UIFont systemFontOfSize:12],NSFontAttributeName,nil];
        NSDictionary *dicS = [NSDictionary dictionaryWithObjectsAndKeys:[UIColor colorWithRed:.33 green:.66 blue:1 alpha:1],NSForegroundColorAttributeName, [UIFont systemFontOfSize:12],NSFontAttributeName ,nil];

        self.speedControl = [[UISegmentedControl alloc] initWithItems:[NSArray arrayWithObjects:speedString count:ARRAY_SIZE(speedString)]];
        [self.speedControl addTarget:self action:@selector(speedControlChange:) forControlEvents:(UIControlEventValueChanged)];
        [self.speedControl setTitleTextAttributes:dicS forState:UIControlStateSelected];
        [self.speedControl setTitleTextAttributes:dic forState:UIControlStateNormal];
        self.speedControl.tintColor = [UIColor clearColor];
        
        self.ratioControl = [[UISegmentedControl alloc] initWithItems:@[@"默认", @"全屏", @"16:9", @"4:3"]];
        [self.ratioControl addTarget:self action:@selector(ratioControlChange:) forControlEvents:(UIControlEventValueChanged)];
        [self.ratioControl setTitleTextAttributes:dicS forState:UIControlStateSelected];
        [self.ratioControl setTitleTextAttributes:dic forState:UIControlStateNormal];
        self.ratioControl.tintColor = [UIColor clearColor];

        UIButton *button[4];
        NSString *buttonTitles[4] = {@"后台播放", @"镜像反转", @"旋转", @"本地缓存"};
        NSString *buttonImages[4] = {@"background_play", @"mirror_swtich", @"rotate", @"save"};
        for (int i = 0; i < 4; i ++) {
            button[i] = [[UIButton alloc] init];
            [button[i] setImage:[UIImage imageNamed:buttonImages[i]] forState:(UIControlStateNormal)];
            [button[i] setTitle:buttonTitles[i] forState:(UIControlStateNormal)];
            button[i].titleLabel.font = [UIFont systemFontOfSize:14];
        }
        
        self.playBackgroundButton = button[0];
        self.mirrorButton = button[1];
        self.rotateButton = button[2];
        self.cacheButton  = button[3];
        
        [self.playBackgroundButton addTarget:self action:@selector(clickPlayBackgroundButton) forControlEvents:(UIControlEventTouchUpInside)];
        [self.mirrorButton addTarget:self action:@selector(clickMirrorButton) forControlEvents:(UIControlEventTouchUpInside)];
        [self.rotateButton addTarget:self action:@selector(clickRotateButton) forControlEvents:(UIControlEventTouchUpInside)];
        [self.cacheButton addTarget:self action:@selector(clickCacheButton) forControlEvents:(UIControlEventTouchUpInside)];
        
        [contentView addSubview:_speedTitleLabel];
        [contentView addSubview:_speedValueLabel];
        [contentView addSubview:_speedControl];
        [contentView addSubview:_ratioControl];
        [contentView addSubview:_playBackgroundButton];
        [contentView addSubview:_mirrorButton];
        [contentView addSubview:_rotateButton];
        [contentView addSubview:_cacheButton];
        
        // 这几句对齐的代码太 low 了，Demo中用用
        [_playBackgroundButton setTitleEdgeInsets:UIEdgeInsetsMake(0, 10, 0, -10)];
        [_mirrorButton setTitleEdgeInsets:UIEdgeInsetsMake(0, 10, 0, -10)];
        [_cacheButton setTitleEdgeInsets:UIEdgeInsetsMake(0, 10, 0, -10)];
        [_rotateButton setTitleEdgeInsets:UIEdgeInsetsMake(0, -5, 0, 0)];
        [_rotateButton setImageEdgeInsets:UIEdgeInsetsMake(0, -30, 0, 0)];
        
        [_cacheButton setTitle:@"缓存已开" forState:(UIControlStateSelected)];
        
        [self.speedTitleLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.top.left.equalTo(contentView).offset(20);
            make.width.equalTo(self.speedTitleLabel.bounds.size.width);
        }];
        
        [self.speedValueLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.equalTo(self.speedTitleLabel.mas_right).offset(5);
            make.centerY.equalTo(self.speedTitleLabel);
        }];
        
        [_speedControl mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.equalTo(self.speedTitleLabel);
            make.right.equalTo(contentView).offset(-20);
            make.top.equalTo(self.speedTitleLabel.mas_bottom).offset(10);
            make.height.equalTo(44);
        }];
        
        [_ratioControl mas_makeConstraints:^(MASConstraintMaker *make) {
            make.height.left.right.equalTo(_speedControl);
            make.top.equalTo(_speedControl.mas_bottom).offset(20);
        }];
        
        [_playBackgroundButton mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.equalTo(_speedControl);
            make.right.equalTo(contentView.mas_centerX);
            make.top.equalTo(_ratioControl.mas_bottom).offset(20);
            make.height.equalTo(50);
        }];
        
        [_mirrorButton mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.equalTo(_playBackgroundButton.mas_right);
            make.size.centerY.equalTo(_playBackgroundButton);
        }];
        
        [_rotateButton mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.height.equalTo(_playBackgroundButton);
            make.top.equalTo(_playBackgroundButton.mas_bottom).offset(20);
        }];
        
        [_cacheButton mas_makeConstraints:^(MASConstraintMaker *make) {
            make.left.right.equalTo(_mirrorButton);
            make.height.equalTo(_mirrorButton);
            make.centerY.equalTo(_rotateButton);
            make.bottom.equalTo(contentView).offset(-20);
        }];
        
        [self resetStatus];
    }
    return self;
}

- (void)resetStatus {
    self.speedControl.selectedSegmentIndex = 2;
    self.ratioControl.selectedSegmentIndex = 0;
    self.playBackgroundButton.selected     = NO;
    self.cacheButton.selected = NO;
    self.speedValueLabel.text =  speedString[self.speedControl.selectedSegmentIndex];
}

- (void)speedControlChange:(UISegmentedControl *)control {
    self.speedValueLabel.text =  speedString[control.selectedSegmentIndex];
    [self.delegate controlView:self speedChange:[speedString[control.selectedSegmentIndex] floatValue]];
}

- (void)ratioControlChange:(UISegmentedControl *)control {
    [self.delegate controlView:self ratioChange:control.selectedSegmentIndex];
}

- (void)clickPlayBackgroundButton {
    self.playBackgroundButton.selected = !self.playBackgroundButton.isSelected;
    [self.delegate controlView:self backgroundPlayChange:self.playBackgroundButton.isSelected];
}

- (void)clickMirrorButton {
    [self.delegate controlViewMirror:self];
}

- (void)clickRotateButton {
    [self.delegate controlViewRotate:self];
}

- (void)clickCacheButton {
    self.cacheButton.selected = [self.delegate controlViewCache:self];
}

- (void)clickCloseButton {
    [self.delegate controlViewClose:self];
}

@end


