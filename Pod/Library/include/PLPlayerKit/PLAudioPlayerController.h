//
//  PLAudioPlayerController.h
//  PLPlayerKit
//
//  Created by 0day on 15/7/17.
//  Copyright (c) 2015年 qgenius. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PLPlayerTypeDefines.h"

@class PLAudioPlayerController;
@protocol PLAudioPlayerControllerDelegate <NSObject>

@optional
- (void)audioPlayerController:(PLAudioPlayerController *)controller playerStateDidChange:(PLPlayerState)status;
- (void)audioPlayerControllerDecoderHasBeenReady:(PLAudioPlayerController *)controller;
- (void)audioPlayerController:(PLAudioPlayerController *)controller failureWithError:(NSError *)error;
- (void)audioPlayerController:(PLAudioPlayerController *)controller positionDidChange:(NSTimeInterval)position;

@end

@interface PLAudioPlayerController : NSObject

+ (instancetype)audioPlayerControllerWithContentURL:(NSURL *)url
                                         parameters:(NSDictionary *)parameters;

@property (nonatomic, weak) id<PLAudioPlayerControllerDelegate> delegate;

@property (nonatomic, assign, getter=isMuted) BOOL  muted;  // default as NO

@property (nonatomic, assign, readonly) PLPlayerState playerState;
@property (nonatomic, readonly, getter=isPlaying) BOOL playing;
@property (nonatomic, assign, readonly) CGFloat audioVolume;
@property (nonatomic, assign, readonly) NSTimeInterval duration;
@property (nonatomic, assign, readonly) NSTimeInterval position;

- (void)play;
- (void)pause;
- (void)forward;
- (void)rewind;

- (void)seekTo:(NSTimeInterval)position;


@end
