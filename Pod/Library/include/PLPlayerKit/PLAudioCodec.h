//
//  PLAudioCodec.h
//  PLPlayerKit
//
//  Created by liang on 8/25/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreAudio/CoreAudioTypes.h>

#import "PLPKPacket.h"
#import "PLPKFrame.h"

@class PLAudioCodec;
@class PLDemux;

typedef PLAudioCodec *(^AudioCodecFactory)(PLDemux *demux);

@protocol PLAudioCodecDelegate <NSObject>

- (void)audioCodec:(PLAudioCodec *)codec AudioDescription:(AudioStreamBasicDescription)asbd frameSize:(int)frameSize;

@end

@interface PLAudioCodec : NSObject

@property (nonatomic, weak) id<PLAudioCodecDelegate> delegate;

+ (instancetype)createCodecFactory:(PLDemux *)demux;
+ (void)registerCodecFactory:(AudioCodecFactory)codecFactory;

- (BOOL)openWithContext:(void *)context;

- (BOOL)openWithAudioStreamBasicDescription:(AudioStreamBasicDescription)asbd;

- (BOOL)close;

- (PLPKFrame *)decode:(PLPKPacket *)packet;

@end
