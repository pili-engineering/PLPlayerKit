//
//  PLDemux.h
//  PLPlayerKit
//
//  Created by liang on 8/23/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMedia/CoreMedia.h>

#import "NSError+Pili.h"

#import "PLPKPacket.h"
#import "PLURLStream.h"

@class PLDemux;

typedef PLDemux *(^DemuxFactory)(PLURLStream *stream);

@protocol PLDemuxDelegate <NSObject>

@optional
/**
 * Callback for checking whether to abort blocking functions.
 * During blocking operations, callback is called.
 * If the callback returns YES, the blocking operation will be aborted.
 */
- (BOOL)IOInterruptDemux:(PLDemux *)demux;

@end

@interface PLDemux : NSObject

@property (nonatomic, weak) id<PLDemuxDelegate> delegate;
@property (nonatomic, readonly) PLURLStream *stream;

@property (nonatomic, assign) void *audioContext;
@property (nonatomic, assign) void *audioStream;
@property (nonatomic, assign) void *videoContext;
@property (nonatomic, assign) void *videoStream;

- (instancetype)initWithStream:(PLURLStream *)stream;

+ (instancetype)createDemux:(NSURL *)url;
+ (void)registerDemuxFactory:(DemuxFactory)demuxFactory;

- (BOOL)open:(NSError * __autoreleasing *)error;
- (BOOL)close;

- (void)seekTo:(CMTime)time;
- (CMTime)duration;
- (CMTime)startTime;

- (PLPKPacket *)readPacket:(NSError * __autoreleasing *)error;

@end
