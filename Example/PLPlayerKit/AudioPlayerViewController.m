//
//  AudioPlayerViewController.m
//  PLPlayerKit
//
//  Created by 0day on 15/8/16.
//  Copyright (c) 2015年 0dayZh. All rights reserved.
//

#import "AudioPlayerViewController.h"
#import <PLPlayerKit/PLPlayerKit.h>

static NSString *states[] = {
    @"Stopped",
    @"Preparing",
    @"Ready",
    @"Caching",
    @"Playing",
    @"Paused"
};

@interface AudioPlayerViewController ()
<
PLAudioPlayerControllerDelegate
>

@property (nonatomic, strong) PLAudioPlayerController   *audioPlayerController;

@end

@implementation AudioPlayerViewController

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
    
    self.audioPlayerController = [PLAudioPlayerController audioPlayerControllerWithContentURL:self.url
                                                                                   parameters:self.parameters];
    self.audioPlayerController.delegate = self;
    
    [[NSNotificationCenter defaultCenter] addObserverForName:PLAudioSessionRouteDidChangeNotification object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification *notif) {
        NSLog(@"Recieved: %@", PLAudioSessionRouteDidChangeNotification);
    }];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:PLAudioSessionDidInterrupteNotification object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification *notif) {
        NSLog(@"Recieved: %@", PLAudioSessionDidInterrupteNotification);
    }];
}

- (void)viewWillDisappear:(BOOL)animated {
    [self.audioPlayerController stop];
    [super viewWillDisappear:animated];
}

#pragma mark - <PLAudioPlayerControllerDelegate>

- (void)audioPlayerController:(PLAudioPlayerController *)controller playerStateDidChange:(PLPlayerState)status {
    NSLog(@"%@", states[status]);
}

@end
