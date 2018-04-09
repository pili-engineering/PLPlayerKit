//
//  BSGConnectivity.m
//
//  Created by Jamie Lynch on 2017-09-04.
//
//  Copyright (c) 2017 Bugsnag, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#import "BSGConnectivity.h"

typedef void (^CallbackBlock)(SCNetworkReachabilityFlags flags);

/**
 * Callback invoked by SCNetworkReachability, which calls an Objective-C block
 * that handles the connection change.
 */
static void BSGConnectivityCallback(SCNetworkReachabilityRef target,
                                    SCNetworkReachabilityFlags flags,
                                    void *info) {
    void (^callbackBlock)(SCNetworkReachabilityFlags) = (__bridge id)(info);
    callbackBlock(flags);
}

@interface BSGConnectivity ()

@property(nonatomic, assign) SCNetworkReachabilityRef reachabilityRef;
@property(nonatomic, strong) dispatch_queue_t serialQueue;
@property(nonatomic, copy) CallbackBlock callbackBlock;

@end

@implementation BSGConnectivity

- (instancetype)initWithURL:(NSURL *)url
                changeBlock:(ConnectivityChange)changeBlock {
    if (self = [super init]) {
        NSString *hostName = [url absoluteString];
        _reachabilityRef = SCNetworkReachabilityCreateWithName(NULL, [hostName UTF8String]);

        _connectivityChangeBlock = [changeBlock copy];
        self.serialQueue = dispatch_queue_create("com.bugsnag.cocoa", NULL);

        __weak id weakSelf = self;
        self.callbackBlock = ^(SCNetworkReachabilityFlags flags) {
          if (weakSelf) {
              [weakSelf connectivityChanged:flags];
          }
        };
    }
    return self;
}

- (void)dealloc {
    [self stopWatchingConnectivity];

    if (self.reachabilityRef) {
        CFRelease(self.reachabilityRef);
        self.reachabilityRef = nil;
    }
}

/**
 * Sets a callback with SCNetworkReachability
 */
- (void)startWatchingConnectivity {
    SCNetworkReachabilityContext context = {
        .version = 0,
        .info = (__bridge void *)self.callbackBlock,
        .retain = CFRetain,
        .release = CFRelease,
    };

    if (self.reachabilityRef) {
        SCNetworkReachabilitySetCallback(self.reachabilityRef,
                                         BSGConnectivityCallback, &context);
        SCNetworkReachabilitySetDispatchQueue(self.reachabilityRef,
                                              self.serialQueue);
    }
}

/**
 * Stops the callback with SCNetworkReachability
 */
- (void)stopWatchingConnectivity {
    if (self.reachabilityRef) {
        SCNetworkReachabilitySetCallback(self.reachabilityRef, NULL, NULL);
        SCNetworkReachabilitySetDispatchQueue(self.reachabilityRef, NULL);
    }
}

- (void)connectivityChanged:(SCNetworkReachabilityFlags)flags {
    BOOL connected = YES;

    if (!(flags & kSCNetworkReachabilityFlagsReachable)) {
        connected = NO;
    }
    if (connected && self.connectivityChangeBlock) { // notify via block
        self.connectivityChangeBlock(self);
    }
}

@end
