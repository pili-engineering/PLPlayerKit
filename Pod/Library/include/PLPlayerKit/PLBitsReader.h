//
//  PLBitsReader.h
//  PLPlayerKit
//
//  Created by liang on 8/19/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#ifndef PLBitsReader_h
#define PLBitsReader_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

unsigned short
PL_DecodeInt16(const char *data);

unsigned int
PL_DecodeInt24(const char *data);

unsigned int
PL_DecodeInt32(const char *data);

char *
PL_EncodeInt16(char *output, char *outend, short nVal);

char *
PL_EncodeInt24(char *output, char *outend, int nVal);

char *
PL_EncodeInt32(char *output, char *outend, int nVal);
    
#ifdef __cplusplus
};
#endif

#endif /* PLBitsReader_h */
