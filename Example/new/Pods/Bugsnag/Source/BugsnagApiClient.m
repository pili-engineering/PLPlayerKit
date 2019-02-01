//
// Created by Jamie Lynch on 04/12/2017.
// Copyright (c) 2017 Bugsnag. All rights reserved.
//

#import "BugsnagApiClient.h"
#import "BugsnagConfiguration.h"
#import "Bugsnag.h"
#import "BugsnagKeys.h"
#import "BugsnagLogger.h"

@interface BSGDelayOperation : NSOperation
@end

@interface BugsnagApiClient()
@property (nonatomic) NSURLSession *generatedSession;
@end

@implementation BugsnagApiClient

- (instancetype)initWithConfig:(BugsnagConfiguration *)configuration
                     queueName:(NSString *)queueName {
    if (self = [super init]) {
        _sendQueue = [NSOperationQueue new];
        _sendQueue.maxConcurrentOperationCount = 1;
        _config = configuration;

        if ([_sendQueue respondsToSelector:@selector(qualityOfService)]) {
            _sendQueue.qualityOfService = NSQualityOfServiceUtility;
        }
        _sendQueue.name = queueName;
    }
    return self;
}

- (void)flushPendingData {
    [self.sendQueue cancelAllOperations];
    BSGDelayOperation *delay = [BSGDelayOperation new];
    NSOperation *deliver = [self deliveryOperation];
    [deliver addDependency:delay];
    [self.sendQueue addOperations:@[delay, deliver] waitUntilFinished:NO];
}

- (NSOperation *)deliveryOperation {
    bsg_log_err(@"Should override deliveryOperation in super class");
    return [NSOperation new];
}

#pragma mark - Delivery


- (void)sendData:(id)data
     withPayload:(NSDictionary *)payload
           toURL:(NSURL *)url
         headers:(NSDictionary *)headers
    onCompletion:(RequestCompletion)onCompletion {

    @try {
        NSError *error = nil;
        NSData *jsonData =
                [NSJSONSerialization dataWithJSONObject:payload
                                                options:0
                                                  error:&error];

        if (jsonData == nil) {
            if (onCompletion) {
                onCompletion(data, NO, error);
            }
            return;
        }
        NSMutableURLRequest *request = [self prepareRequest:url headers:headers];

        if ([NSURLSession class]) {
            NSURLSession *session = [self prepareSession];
            NSURLSessionTask *task = [session
                    uploadTaskWithRequest:request
                                 fromData:jsonData
                        completionHandler:^(NSData *_Nullable responseBody,
                                NSURLResponse *_Nullable response,
                                NSError *_Nullable requestErr) {
                            if (onCompletion) {
                                onCompletion(data, requestErr == nil, requestErr);
                            }
                        }];
            [task resume];
        } else {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            NSURLResponse *response = nil;
            request.HTTPBody = jsonData;
            [NSURLConnection sendSynchronousRequest:request
                                  returningResponse:&response
                                              error:&error];
            if (onCompletion) {
                onCompletion(data, error == nil, error);
            }
#pragma clang diagnostic pop
        }
    } @catch (NSException *exception) {
        if (onCompletion) {
            onCompletion(data, NO,
                    [NSError            errorWithDomain:exception.reason
                                        code:420
                                    userInfo:@{BSGKeyException: exception}]);
        }
    }
}

- (NSURLSession *)prepareSession {
    NSURLSession *session = [Bugsnag configuration].session;
    if (session) {
        return session;
    } else {
        if (self.generatedSession) {
            _generatedSession = [NSURLSession
                    sessionWithConfiguration:[NSURLSessionConfiguration
                            defaultSessionConfiguration]];
        }
        return self.generatedSession;
    }
}

- (NSMutableURLRequest *)prepareRequest:(NSURL *)url
                                headers:(NSDictionary *)headers {
    NSMutableURLRequest *request = [NSMutableURLRequest
            requestWithURL:url
               cachePolicy:NSURLRequestReloadIgnoringLocalCacheData
           timeoutInterval:15];
    request.HTTPMethod = @"POST";
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];

    for (NSString *key in [headers allKeys]) {
        [request setValue:headers[key] forHTTPHeaderField:key];
    }
    return request;
}

- (void)dealloc {
    [self.sendQueue cancelAllOperations];
}

@end

@implementation BSGDelayOperation
const NSTimeInterval BSG_SEND_DELAY_SECS = 1;

- (void)main {
    [NSThread sleepForTimeInterval:BSG_SEND_DELAY_SECS];
}

@end
