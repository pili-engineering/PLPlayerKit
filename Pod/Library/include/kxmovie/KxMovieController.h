//
//  KxMovieController.h
//  kxmovie
//
//  Created by 0day on 15/5/6.
//
//

#import <Foundation/Foundation.h>

@class KxMovieDecoder;

extern NSString * const KxMovieParameterMinBufferedDuration;    // Float
extern NSString * const KxMovieParameterMaxBufferedDuration;    // Float
extern NSString * const KxMovieParameterDisableDeinterlacing;   // BOOL
extern NSString * const KxMovieParameterFrameViewContentMode;   // default as UIViewContentModeScaleAspectFit.

@class KxMovieController;
@protocol KxMovieControllerDelegate <NSObject>

- (void)movieController:(KxMovieController *)controller failureWithError:(NSError *)error;

@end

@interface KxMovieController : NSObject

+ (id)movieControllerWithContentPath:(NSString *)path
                          parameters:(NSDictionary *)parameters;

@property (nonatomic, weak) id<KxMovieControllerDelegate> delegate;

@property (nonatomic, readonly, strong) UIView    *playerView;
@property (nonatomic, assign) BOOL userInteractionEnable;   // default as YES
@property (nonatomic, assign, getter=isMuted) BOOL  muted;  // default as NO

@property (nonatomic, readonly, getter=isPlaying) BOOL playing;
@property (nonatomic, assign, readonly) CGFloat audioVolume;
@property (nonatomic, assign, readonly) CGFloat duration;
@property (nonatomic, assign, readonly) CGFloat position;

- (void)play;
- (void)pause;
- (void)forward;
- (void)rewind;

- (void)setMoviePosition:(CGFloat)position;

@end
