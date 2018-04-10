//
// Created by Jamie Lynch on 04/12/2017.
// Copyright (c) 2017 Bugsnag. All rights reserved.
//

#import <Foundation/Foundation.h>

@class BugsnagConfiguration;

typedef void (^RequestCompletion)(id data, BOOL success, NSError *error);

@interface BugsnagApiClient : NSObject

- (instancetype)initWithConfig:(BugsnagConfiguration *)configuration
                     queueName:(NSString *)queueName;

/**
 * Send outstanding reports
 */
- (void)flushPendingData;

- (NSOperation *)deliveryOperation;

- (void)sendData:(id)data
     withPayload:(NSDictionary *)payload
           toURL:(NSURL *)url
         headers:(NSDictionary *)headers
    onCompletion:(RequestCompletion)onCompletion;

@property(readonly) NSOperationQueue *sendQueue;
@property(readonly) BugsnagConfiguration *config;


@end
