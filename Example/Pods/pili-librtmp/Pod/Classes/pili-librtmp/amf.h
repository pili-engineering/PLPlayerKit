#ifndef __AMF_H__
#define __AMF_H__
/*
 *      Copyright (C) 2005-2008 Team XBMC
 *      http://www.xbmc.org
 *      Copyright (C) 2008-2009 Andrej Stepanchuk
 *      Copyright (C) 2009-2010 Howard Chu
 *
 *  This file is part of librtmp.
 *
 *  librtmp is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1,
 *  or (at your option) any later version.
 *
 *  librtmp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with librtmp see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *  http://www.gnu.org/copyleft/lgpl.html
 */

#include <stdint.h>

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PILI_AMF_NUMBER = 0,
    PILI_AMF_BOOLEAN,
    PILI_AMF_STRING,
    PILI_AMF_OBJECT,
    PILI_AMF_MOVIECLIP, /* reserved, not used */
    PILI_AMF_NULL,
    PILI_AMF_UNDEFINED,
    PILI_AMF_REFERENCE,
    PILI_AMF_ECMA_ARRAY,
    PILI_AMF_OBJECT_END,
    PILI_AMF_STRICT_ARRAY,
    PILI_AMF_DATE,
    PILI_AMF_LONG_STRING,
    PILI_AMF_UNSUPPORTED,
    PILI_AMF_RECORDSET, /* reserved, not used */
    PILI_AMF_XML_DOC,
    PILI_AMF_TYPED_OBJECT,
    PILI_AMF_AVMPLUS, /* switch to AMF3 */
    PILI_AMF_INVALID = 0xff
} PILI_AMFDataType;

typedef enum {
    PILI_AMF3_UNDEFINED = 0,
    PILI_AMF3_NULL,
    PILI_AMF3_FALSE,
    PILI_AMF3_TRUE,
    PILI_AMF3_INTEGER,
    PILI_AMF3_DOUBLE,
    PILI_AMF3_STRING,
    PILI_AMF3_XML_DOC,
    PILI_AMF3_DATE,
    PILI_AMF3_ARRAY,
    PILI_AMF3_OBJECT,
    PILI_AMF3_XML,
    PILI_AMF3_BYTE_ARRAY
} PILI_AMF3DataType;

typedef struct PILI_AVal {
    char *av_val;
    int av_len;
} PILI_AVal;
#define AVC(str) \
    { str, sizeof(str) - 1 }
#define AVMATCH(a1, a2)              \
    ((a1)->av_len == (a2)->av_len && \
     !memcmp((a1)->av_val, (a2)->av_val, (a1)->av_len))

struct PILI_AMFObjectProperty;

typedef struct PILI_AMFObject {
    int o_num;
    struct PILI_AMFObjectProperty *o_props;
} PILI_AMFObject;

typedef struct PILI_AMFObjectProperty {
    PILI_AVal p_name;
    PILI_AMFDataType p_type;
    union {
        double p_number;
        PILI_AVal p_aval;
        PILI_AMFObject p_object;
    } p_vu;
    int16_t p_UTCoffset;
} PILI_AMFObjectProperty;

char *PILI_AMF_EncodeString(char *output, char *outend, const PILI_AVal *str);
char *PILI_AMF_EncodeNumber(char *output, char *outend, double dVal);
char *PILI_AMF_EncodeInt16(char *output, char *outend, short nVal);
char *PILI_AMF_EncodeInt24(char *output, char *outend, int nVal);
char *PILI_AMF_EncodeInt32(char *output, char *outend, int nVal);
char *PILI_AMF_EncodeBoolean(char *output, char *outend, int bVal);

/* Shortcuts for PILI_AMFProp_Encode */
char *PILI_AMF_EncodeNamedString(char *output, char *outend, const PILI_AVal *name,
                            const PILI_AVal *value);
char *PILI_AMF_EncodeNamedNumber(char *output, char *outend, const PILI_AVal *name,
                            double dVal);
char *PILI_AMF_EncodeNamedBoolean(char *output, char *outend, const PILI_AVal *name,
                             int bVal);

unsigned short PILI_AMF_DecodeInt16(const char *data);
unsigned int PILI_AMF_DecodeInt24(const char *data);
unsigned int PILI_AMF_DecodeInt32(const char *data);
void PILI_AMF_DecodeString(const char *data, PILI_AVal *str);
void PILI_AMF_DecodeLongString(const char *data, PILI_AVal *str);
int PILI_AMF_DecodeBoolean(const char *data);
double PILI_AMF_DecodeNumber(const char *data);

char *PILI_AMF_Encode(PILI_AMFObject *obj, char *pBuffer, char *pBufEnd);
int PILI_AMF_Decode(PILI_AMFObject *obj, const char *pBuffer, int nSize, int bDecodeName);
int PILI_AMF_DecodeArray(PILI_AMFObject *obj, const char *pBuffer, int nSize,
                    int nArrayLen, int bDecodeName);
int PILI_AMF3_Decode(PILI_AMFObject *obj, const char *pBuffer, int nSize,
                int bDecodeName);
void PILI_AMF_Dump(PILI_AMFObject *obj);
void PILI_AMF_Reset(PILI_AMFObject *obj);

void PILI_AMF_AddProp(PILI_AMFObject *obj, const PILI_AMFObjectProperty *prop);
int PILI_AMF_CountProp(PILI_AMFObject *obj);
PILI_AMFObjectProperty *PILI_AMF_GetProp(PILI_AMFObject *obj, const PILI_AVal *name, int nIndex);

PILI_AMFDataType PILI_AMFProp_GetType(PILI_AMFObjectProperty *prop);
void PILI_AMFProp_SetNumber(PILI_AMFObjectProperty *prop, double dval);
void PILI_AMFProp_SetBoolean(PILI_AMFObjectProperty *prop, int bflag);
void PILI_AMFProp_SetString(PILI_AMFObjectProperty *prop, PILI_AVal *str);
void PILI_AMFProp_SetObject(PILI_AMFObjectProperty *prop, PILI_AMFObject *obj);

void PILI_AMFProp_GetName(PILI_AMFObjectProperty *prop, PILI_AVal *name);
void PILI_AMFProp_SetName(PILI_AMFObjectProperty *prop, PILI_AVal *name);
double PILI_AMFProp_GetNumber(PILI_AMFObjectProperty *prop);
int PILI_AMFProp_GetBoolean(PILI_AMFObjectProperty *prop);
void PILI_AMFProp_GetString(PILI_AMFObjectProperty *prop, PILI_AVal *str);
void PILI_AMFProp_GetObject(PILI_AMFObjectProperty *prop, PILI_AMFObject *obj);

int PILI_AMFProp_IsValid(PILI_AMFObjectProperty *prop);

char *PILI_AMFProp_Encode(PILI_AMFObjectProperty *prop, char *pBuffer, char *pBufEnd);
int PILI_AMF3Prop_Decode(PILI_AMFObjectProperty *prop, const char *pBuffer, int nSize,
                    int bDecodeName);
int PILI_AMFProp_Decode(PILI_AMFObjectProperty *prop, const char *pBuffer, int nSize,
                   int bDecodeName);

void PILI_AMFProp_Dump(PILI_AMFObjectProperty *prop);
void PILI_AMFProp_Reset(PILI_AMFObjectProperty *prop);

typedef struct PILI_AMF3ClassDef {
    PILI_AVal cd_name;
    char cd_externalizable;
    char cd_dynamic;
    int cd_num;
    PILI_AVal *cd_props;
} PILI_AMF3ClassDef;

void PILI_AMF3CD_AddProp(PILI_AMF3ClassDef *cd, PILI_AVal *prop);
PILI_AVal *PILI_AMF3CD_GetProp(PILI_AMF3ClassDef *cd, int idx);

#ifdef __cplusplus
}
#endif

#endif /* __AMF_H__ */
