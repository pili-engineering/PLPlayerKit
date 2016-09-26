//
//  PLURLStream.h
//  PLPlayerKit
//
//  Created by liang on 8/22/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "NSError+Pili.h"

@class PLURLStream;

typedef PLURLStream *(^StreamFactory)(NSURL *url);

@protocol PLURLStreamDelegate <NSObject>

@optional
/**
 * Callback for checking whether to abort blocking functions.
 * During blocking operations, callback is called.
 * If the callback returns YES, the blocking operation will be aborted.
 */
- (BOOL)IOInterruptStream:(PLURLStream *)stream;

@end

@interface PLURLStream : NSObject

@property (nonatomic, weak) id<PLURLStreamDelegate> delegate;
@property (nonatomic, readonly) NSURL *url;
@property (nonatomic, strong) NSString *streamID;

- (instancetype)initWithURL:(NSURL *)url;

+ (instancetype)createURLStream:(NSURL *)url;
+ (void)registerURLStreamFactory:(StreamFactory)streamFactory;

- (BOOL)open:(NSError * __autoreleasing *)error;
- (BOOL)close;

- (size_t)read:(uint8_t *)buf size:(size_t)size error:(NSError * __autoreleasing *)error;
- (size_t)write:(const uint8_t *)buf size:(size_t)size error:(NSError * __autoreleasing *)error;

@end
