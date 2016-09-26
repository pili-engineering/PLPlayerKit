//
//  PLFLVDemux.m
//  PLPlayerKit
//
//  Created by liang on 8/23/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#include "PLBitsReader.h"
#import "PLFLVDemux.h"

static const size_t kBufSize = 512 * 1024;

typedef NS_ENUM(NSUInteger, PLTagType) {
    PLTagAudio = 8,
    PLTagVideo = 9,
    PLTagScriptData = 18,
};

@interface PLFLVDemux ()
{
    uint8_t *_buf;
}

@end

@implementation PLFLVDemux

- (instancetype)initWithStream:(PLURLStream *)stream {
    self = [super initWithStream:stream];
    if (self) {
        _buf = malloc(kBufSize);
    }
    return self;
}

- (void)dealloc {
    if (_buf) {
        free(_buf);
        _buf = NULL;
    }
}

- (PLPKPacket *)readPacket:(NSError *__autoreleasing *)error {
    size_t ret = [self.stream read:_buf size:kBufSize error:error];
    if (ret > 0) {
        size_t total = ret;
        PLPKPacket *packet = [[PLPKPacket alloc] initWithFLVData:_buf size:total];
        return packet;
    }
    return nil;
}

@end
