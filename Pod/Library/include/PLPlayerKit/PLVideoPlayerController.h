//
//  PLVideoPlayerController.h
//  PLPlayerKit
//
//  Created on 15/5/6.
//  Copyright (c) 2015年 Pili Engineering. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PLPlayerTypeDefines.h"

@class PLVideoPlayerController;
@protocol PLVideoPlayerControllerDelegate <NSObject>

- (void)videoPlayerController:(PLVideoPlayerController *)playerController failureWithError:(NSError *)error;

@end

@interface PLVideoPlayerController : NSObject

+ (instancetype)videoPlayerControllerWithContentURL:(NSURL *)url
                                         parameters:(NSDictionary *)parameters;

@property (nonatomic, weak) id<PLVideoPlayerControllerDelegate> delegate;
@property (nonatomic, readonly, strong) UIView    *playerView;
@property (nonatomic, readonly, getter=isPlaying) BOOL playing;
@property (nonatomic, assign) BOOL userInteractionEnable;   // default as YES

- (void)play;
- (void)pause;

@end
