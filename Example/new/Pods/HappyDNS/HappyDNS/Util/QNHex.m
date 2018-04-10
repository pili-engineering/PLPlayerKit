//
//  QNHex.m
//  HappyDNS
//
//  Created by bailong on 15/7/31.
//  Copyright (c) 2015年 Qiniu Cloud Storage. All rights reserved.
//

#import "QNHex.h"

static char DIGITS_LOWER[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

static char DIGITS_UPPER[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static int hexDigit(char c) {
    int result = -1;
    if ('0' <= c && c <= '9') {
        result = c - '0';
    } else if ('a' <= c && c <= 'f') {
        result = 10 + (c - 'a');
    } else if ('A' <= c && c <= 'F') {
        result = 10 + (c - 'A');
    }
    return result;
}

static char *decodeHex(const char *data, int size) {
    if ((size & 0x01) != 0) {
        return NULL;
    }
    char *output = malloc(size / 2);
    int outLimit = 0;
    for (int i = 0, j = 0; j < size; i++) {
        int f = hexDigit(data[j]);
        if (f < 0) {
            outLimit = 1;
            break;
        }
        j++;
        int f2 = hexDigit(data[j]);
        if (f2 < 0) {
            outLimit = 1;
            break;
        }
        f = (f << 4) | f2;
        j++;
        output[i] = (char)(f & 0xff);
    }
    if (outLimit) {
        free(output);
        return NULL;
    }
    return output;
}

static char *encodeHexInternal(char *output_buf, const char *data, int size, char hexTable[]) {
    for (int i = 0, j = 0; i < size; i++) {
        output_buf[j++] = hexTable[((0XF0 & data[i]) >> 4) & 0X0F];
        output_buf[j++] = hexTable[((0X0F & data[i])) & 0X0F];
    }
    return output_buf;
}

static char *encodeHex(const char *data, int size, char hexTable[]) {
    char *output = malloc(size * 2);
    return encodeHexInternal(output, data, size, hexTable);
}

char *qn_encodeHexData(char *buff, const char *data, int data_size, BOOL up) {
    char *hexTable = DIGITS_UPPER;
    if (!up) {
        hexTable = DIGITS_LOWER;
    }
    return encodeHexInternal(buff, data, data_size, hexTable);
}

@implementation QNHex

+ (NSString *)encodeHexData:(NSData *)data {
    char *e = encodeHex(data.bytes, (int)data.length, DIGITS_UPPER);
    NSString *str = [[NSString alloc] initWithBytes:e length:data.length * 2 encoding:NSASCIIStringEncoding];
    free(e);
    return str;
}

+ (NSString *)encodeHexString:(NSString *)str {
    return [QNHex encodeHexData:[str dataUsingEncoding:NSUTF8StringEncoding]];
}

+ (NSData *)decodeHexString:(NSString *)hex {
    char *d = decodeHex(hex.UTF8String, (int)hex.length);
    if (d == NULL) {
        return nil;
    }
    NSData *data = [NSData dataWithBytes:d length:hex.length / 2];
    free(d);
    return data;
}

+ (NSString *)decodeHexToString:(NSString *)hex {
    NSData *data = [QNHex decodeHexString:hex];
    if (data == nil) {
        return nil;
    }
    return [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
}

@end
