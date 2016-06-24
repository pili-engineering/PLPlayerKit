//
//  QNDes.m
//  HappyDNS
//
//  Created by bailong on 15/8/1.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNDes.h"

#import <CommonCrypto/CommonCryptor.h>

@interface QNDes ()
@property (nonatomic, strong) NSData *key;
@end

@implementation QNDes

- (NSData *)encrypt:(NSData *)data {
    const void *input = data.bytes;
    size_t inputSize = data.length;

    size_t bufferSize = (inputSize + kCCBlockSize3DES) & ~(kCCBlockSize3DES - 1);
    uint8_t *buffer = malloc(bufferSize * sizeof(uint8_t));
    memset((void *)buffer, 0x0, bufferSize);
    size_t movedBytes = 0;

    const void *vkey = _key.bytes;

    CCCryptorStatus ccStatus = CCCrypt(kCCEncrypt,
                                       kCCAlgorithmDES,
                                       kCCOptionECBMode | kCCOptionPKCS7Padding,
                                       vkey,
                                       kCCKeySizeDES,
                                       NULL,
                                       input,
                                       inputSize,
                                       (void *)buffer,
                                       bufferSize,
                                       &movedBytes);
    if (ccStatus != kCCSuccess) {
        NSLog(@"error code %d", ccStatus);
        free(buffer);
        return nil;
    }
    NSData *encrypted = [NSData dataWithBytes:(const void *)buffer length:(NSUInteger)movedBytes];
    free(buffer);
    return encrypted;
}

- (NSData *)decrpyt:(NSData *)raw {
    const void *input = raw.bytes;
    size_t inputSize = raw.length;

    size_t bufferSize = 1024;
    uint8_t *buffer = malloc(bufferSize * sizeof(uint8_t));
    memset((void *)buffer, 0x0, bufferSize);
    size_t movedBytes = 0;

    const void *vkey = _key.bytes;

    CCCryptorStatus ccStatus = CCCrypt(kCCDecrypt,
                                       kCCAlgorithmDES,
                                       kCCOptionECBMode | kCCOptionPKCS7Padding,
                                       vkey,
                                       kCCKeySizeDES,
                                       NULL,
                                       input,
                                       inputSize,
                                       (void *)buffer,
                                       bufferSize,
                                       &movedBytes);

    if (ccStatus != kCCSuccess) {
        NSLog(@"error code %d", ccStatus);
        free(buffer);
        return nil;
    }

    NSData *decrypted = [NSData dataWithBytes:(const void *)buffer length:(NSUInteger)movedBytes];
    free(buffer);
    return decrypted;
}

- (instancetype)init:(NSData *)key {
    if (self = [super init]) {
        _key = key;
    }
    return self;
}

@end
