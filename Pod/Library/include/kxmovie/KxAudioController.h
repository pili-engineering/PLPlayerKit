//
//  KxAudioController.h
//  kxmovie
//
//  Created by 0day on 15/7/17.
//
//

#import <Foundation/Foundation.h>
#import "KxTypeDefines.h"

@class KxAudioController;
@protocol KxAudioControllerDelegate <NSObject>

@optional
- (void)audioController:(KxAudioController *)controller playerStateDidChange:(KxPlayerState)status;
- (void)audioControllerDecoderHasBeenReady:(KxAudioController *)controller;
- (void)audioController:(KxAudioController *)controller failureWithError:(NSError *)error;
- (void)audioController:(KxAudioController *)controller positionDidChange:(NSTimeInterval)position;

@end

@interface KxAudioController : NSObject

+ (id)audioControllerWithContentPath:(NSString *)path
                          parameters:(NSDictionary *)parameters;

@property (nonatomic, weak) id<KxAudioControllerDelegate> delegate;

@property (nonatomic, assign, getter=isMuted) BOOL  muted;  // default as NO

@property (nonatomic, assign, readonly) KxPlayerState playerState;
@property (nonatomic, readonly, getter=isPlaying) BOOL playing;
@property (nonatomic, assign, readonly) CGFloat audioVolume;
@property (nonatomic, assign, readonly) NSTimeInterval duration;
@property (nonatomic, assign, readonly) NSTimeInterval position;

/// 超时时长，默认为 8s
@property (nonatomic, assign) NSTimeInterval    timeout;

- (void)prepareToPlayWithCompletion:(void (^)(BOOL success))handler;
- (void)play;
- (void)pause;
- (void)stop;
- (void)forward;
- (void)rewind;

- (void)seekTo:(NSTimeInterval)position;

@end
