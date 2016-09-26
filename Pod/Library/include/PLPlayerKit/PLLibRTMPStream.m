//
//  PLLibRTMPStream.m
//  PLPlayerKit
//
//  Created by liang on 8/23/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#include "rtmp.h"
#import "PLLibRTMPStream.h"
#import "NSError+Pili.h"

@interface PLLibRTMPStream ()
{
    PILI_RTMP  *_rtmp;
}

@end

@implementation PLLibRTMPStream

- (void)IOInterruptStream:(NSError * __autoreleasing *)error
                errorCode:(PLPlayerError)errorCode
                  message:(const char *)message {
    if ([self.delegate respondsToSelector:@selector(IOInterruptStream:)]) {
        if ([self.delegate IOInterruptStream:self]) {
            if (error) {
                *error = [NSError errorWithPlayerError:errorCode
                                               message:message];
            }
        }
    }
}

- (BOOL)open:(NSError * __autoreleasing *)error {
    _rtmp = PILI_RTMP_Alloc();
    PILI_RTMP_Init(_rtmp);
    
    RTMPError err;
    err.message = NULL;
    printf("librtmp setup url\n");
    const char *url = [self.url.absoluteString cStringUsingEncoding:NSUTF8StringEncoding];
    if (!PILI_RTMP_SetupURL(_rtmp, url, &err)) {
        goto fail;
    }
    [self IOInterruptStream:error
                  errorCode:PLPlayerErrorInputTimeout
                    message:"lib rtmp setup url timeout"];
    
    printf("librtmp connect\n");
    if (!PILI_RTMP_Connect(_rtmp, NULL, &err)) {
        goto fail;
    }
    [self IOInterruptStream:error
                  errorCode:PLPlayerErrorInputTimeout
                    message:"lib rtmp connect timeout"];
    
    printf("librtmp connect stream\n");
    if (!PILI_RTMP_ConnectStream(_rtmp, 0, &err)) {
        goto fail;
    }
    [self IOInterruptStream:error
                  errorCode:PLPlayerErrorInputTimeout
                    message:"lib rtmp connect stream timeout"];
    
    printf("librtmp open success\n");
    
    const char *reqID = PILI_RTMP_GetReqId(_rtmp);
    if (reqID) {
        self.streamID = [NSString stringWithFormat:@"%s", reqID];
    }
    
    return YES;
    
fail:
    if (error) {
        *error = [NSError errorWithPlayerError:err.code
                                       message:err.message];
        PILI_RTMPError_Free(&err);
    }

    return NO;
}

- (BOOL)close {
    if (_rtmp) {
        PILI_RTMP_Close(_rtmp, NULL);
        PILI_RTMP_Free(_rtmp);
        _rtmp = NULL;
    }
    return YES;
}

- (size_t)read:(uint8_t *)buf size:(size_t)size error:(NSError *__autoreleasing *)error {
    size_t ret = PILI_RTMP_Read(_rtmp, buf, size);
    if (ret <= 0) {
        printf("librtmp read ret : %ld\n", ret);
        if (error) {
            if (error) {
                if (_rtmp->m_error) {
                    *error = [NSError errorWithPlayerError:_rtmp->m_error->code
                                                   message:_rtmp->m_error->message];
                } else {
                    *error = [NSError errorWithPlayerError:PLPLayerErrorInputReadError
                                                   message:"librtmp stream read error"];
                }
            }
        }
    }
    
    return ret;
}

@end
