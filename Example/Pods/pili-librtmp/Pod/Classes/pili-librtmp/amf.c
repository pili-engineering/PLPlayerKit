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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "amf.h"
#include "bytes.h"
#include "log.h"
#include "rtmp_sys.h"

static const PILI_AMFObjectProperty AMFProp_Invalid = {{0, 0}, PILI_AMF_INVALID};
static const PILI_AVal AV_empty = {0, 0};

/* Data is Big-Endian */
unsigned short
    PILI_AMF_DecodeInt16(const char *data) {
    unsigned char *c = (unsigned char *)data;
    unsigned short val;
    val = (c[0] << 8) | c[1];
    return val;
}

unsigned int
    PILI_AMF_DecodeInt24(const char *data) {
    unsigned char *c = (unsigned char *)data;
    unsigned int val;
    val = (c[0] << 16) | (c[1] << 8) | c[2];
    return val;
}

unsigned int
    PILI_AMF_DecodeInt32(const char *data) {
    unsigned char *c = (unsigned char *)data;
    unsigned int val;
    val = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
    return val;
}

void PILI_AMF_DecodeString(const char *data, PILI_AVal *bv) {
    bv->av_len = PILI_AMF_DecodeInt16(data);
    bv->av_val = (bv->av_len > 0) ? (char *)data + 2 : NULL;
}

void PILI_AMF_DecodeLongString(const char *data, PILI_AVal *bv) {
    bv->av_len = PILI_AMF_DecodeInt32(data);
    bv->av_val = (bv->av_len > 0) ? (char *)data + 4 : NULL;
}

double
    PILI_AMF_DecodeNumber(const char *data) {
    double dVal;
#if __FLOAT_WORD_ORDER == __BYTE_ORDER
#if __BYTE_ORDER == __BIG_ENDIAN
    memcpy(&dVal, data, 8);
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned char *ci, *co;
    ci = (unsigned char *)data;
    co = (unsigned char *)&dVal;
    co[0] = ci[7];
    co[1] = ci[6];
    co[2] = ci[5];
    co[3] = ci[4];
    co[4] = ci[3];
    co[5] = ci[2];
    co[6] = ci[1];
    co[7] = ci[0];
#endif
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN /* __FLOAT_WORD_ORER == __BIG_ENDIAN */
    unsigned char *ci, *co;
    ci = (unsigned char *)data;
    co = (unsigned char *)&dVal;
    co[0] = ci[3];
    co[1] = ci[2];
    co[2] = ci[1];
    co[3] = ci[0];
    co[4] = ci[7];
    co[5] = ci[6];
    co[6] = ci[5];
    co[7] = ci[4];
#else /* __BYTE_ORDER == __BIG_ENDIAN && __FLOAT_WORD_ORER == __LITTLE_ENDIAN */
    unsigned char *ci, *co;
    ci = (unsigned char *)data;
    co = (unsigned char *)&dVal;
    co[0] = ci[4];
    co[1] = ci[5];
    co[2] = ci[6];
    co[3] = ci[7];
    co[4] = ci[0];
    co[5] = ci[1];
    co[6] = ci[2];
    co[7] = ci[3];
#endif
#endif
    return dVal;
}

int PILI_AMF_DecodeBoolean(const char *data) {
    return *data != 0;
}

char *
    PILI_AMF_EncodeInt16(char *output, char *outend, short nVal) {
    if (output + 2 > outend)
        return NULL;

    output[1] = nVal & 0xff;
    output[0] = nVal >> 8;
    return output + 2;
}

char *
    PILI_AMF_EncodeInt24(char *output, char *outend, int nVal) {
    if (output + 3 > outend)
        return NULL;

    output[2] = nVal & 0xff;
    output[1] = nVal >> 8;
    output[0] = nVal >> 16;
    return output + 3;
}

char *
    PILI_AMF_EncodeInt32(char *output, char *outend, int nVal) {
    if (output + 4 > outend)
        return NULL;

    output[3] = nVal & 0xff;
    output[2] = nVal >> 8;
    output[1] = nVal >> 16;
    output[0] = nVal >> 24;
    return output + 4;
}

char *
    PILI_AMF_EncodeString(char *output, char *outend, const PILI_AVal *bv) {
    if ((bv->av_len < 65536 && output + 1 + 2 + bv->av_len > outend) ||
        output + 1 + 4 + bv->av_len > outend)
        return NULL;

    if (bv->av_len < 65536) {
        *output++ = PILI_AMF_STRING;

        output = PILI_AMF_EncodeInt16(output, outend, bv->av_len);
    } else {
        *output++ = PILI_AMF_LONG_STRING;

        output = PILI_AMF_EncodeInt32(output, outend, bv->av_len);
    }
    memcpy(output, bv->av_val, bv->av_len);
    output += bv->av_len;

    return output;
}

char *
    PILI_AMF_EncodeNumber(char *output, char *outend, double dVal) {
    if (output + 1 + 8 > outend)
        return NULL;

    *output++ = PILI_AMF_NUMBER; /* type: Number */

#if __FLOAT_WORD_ORDER == __BYTE_ORDER
#if __BYTE_ORDER == __BIG_ENDIAN
    memcpy(output, &dVal, 8);
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    {
        unsigned char *ci, *co;
        ci = (unsigned char *)&dVal;
        co = (unsigned char *)output;
        co[0] = ci[7];
        co[1] = ci[6];
        co[2] = ci[5];
        co[3] = ci[4];
        co[4] = ci[3];
        co[5] = ci[2];
        co[6] = ci[1];
        co[7] = ci[0];
    }
#endif
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN /* __FLOAT_WORD_ORER == __BIG_ENDIAN */
    {
        unsigned char *ci, *co;
        ci = (unsigned char *)&dVal;
        co = (unsigned char *)output;
        co[0] = ci[3];
        co[1] = ci[2];
        co[2] = ci[1];
        co[3] = ci[0];
        co[4] = ci[7];
        co[5] = ci[6];
        co[6] = ci[5];
        co[7] = ci[4];
    }
#else /* __BYTE_ORDER == __BIG_ENDIAN && __FLOAT_WORD_ORER == __LITTLE_ENDIAN */
    {
        unsigned char *ci, *co;
        ci = (unsigned char *)&dVal;
        co = (unsigned char *)output;
        co[0] = ci[4];
        co[1] = ci[5];
        co[2] = ci[6];
        co[3] = ci[7];
        co[4] = ci[0];
        co[5] = ci[1];
        co[6] = ci[2];
        co[7] = ci[3];
    }
#endif
#endif

    return output + 8;
}

char *
    PILI_AMF_EncodeBoolean(char *output, char *outend, int bVal) {
    if (output + 2 > outend)
        return NULL;

    *output++ = PILI_AMF_BOOLEAN;

    *output++ = bVal ? 0x01 : 0x00;

    return output;
}

char *
    PILI_AMF_EncodeNamedString(char *output, char *outend, const PILI_AVal *strName, const PILI_AVal *strValue) {
    if (output + 2 + strName->av_len > outend)
        return NULL;
    output = PILI_AMF_EncodeInt16(output, outend, strName->av_len);

    memcpy(output, strName->av_val, strName->av_len);
    output += strName->av_len;

    return PILI_AMF_EncodeString(output, outend, strValue);
}

char *
    PILI_AMF_EncodeNamedNumber(char *output, char *outend, const PILI_AVal *strName, double dVal) {
    if (output + 2 + strName->av_len > outend)
        return NULL;
    output = PILI_AMF_EncodeInt16(output, outend, strName->av_len);

    memcpy(output, strName->av_val, strName->av_len);
    output += strName->av_len;

    return PILI_AMF_EncodeNumber(output, outend, dVal);
}

char *
    PILI_AMF_EncodeNamedBoolean(char *output, char *outend, const PILI_AVal *strName, int bVal) {
    if (output + 2 + strName->av_len > outend)
        return NULL;
    output = PILI_AMF_EncodeInt16(output, outend, strName->av_len);

    memcpy(output, strName->av_val, strName->av_len);
    output += strName->av_len;

    return PILI_AMF_EncodeBoolean(output, outend, bVal);
}

void PILI_AMFProp_GetName(PILI_AMFObjectProperty *prop, PILI_AVal *name) {
    *name = prop->p_name;
}

void PILI_AMFProp_SetName(PILI_AMFObjectProperty *prop, PILI_AVal *name) {
    prop->p_name = *name;
}

PILI_AMFDataType
    PILI_AMFProp_GetType(PILI_AMFObjectProperty *prop) {
    return prop->p_type;
}

double
    PILI_AMFProp_GetNumber(PILI_AMFObjectProperty *prop) {
    return prop->p_vu.p_number;
}

int PILI_AMFProp_GetBoolean(PILI_AMFObjectProperty *prop) {
    return prop->p_vu.p_number != 0;
}

void PILI_AMFProp_GetString(PILI_AMFObjectProperty *prop, PILI_AVal *str) {
    *str = prop->p_vu.p_aval;
}

void PILI_AMFProp_GetObject(PILI_AMFObjectProperty *prop, PILI_AMFObject *obj) {
    *obj = prop->p_vu.p_object;
}

int PILI_AMFProp_IsValid(PILI_AMFObjectProperty *prop) {
    return prop->p_type != PILI_AMF_INVALID;
}

char *
    PILI_AMFProp_Encode(PILI_AMFObjectProperty *prop, char *pBuffer, char *pBufEnd) {
    if (prop->p_type == PILI_AMF_INVALID)
        return NULL;

    if (prop->p_type != PILI_AMF_NULL && pBuffer + prop->p_name.av_len + 2 + 1 >= pBufEnd)
        return NULL;

    if (prop->p_type != PILI_AMF_NULL && prop->p_name.av_len) {
        *pBuffer++ = prop->p_name.av_len >> 8;
        *pBuffer++ = prop->p_name.av_len & 0xff;
        memcpy(pBuffer, prop->p_name.av_val, prop->p_name.av_len);
        pBuffer += prop->p_name.av_len;
    }

    switch (prop->p_type) {
        case PILI_AMF_NUMBER:
            pBuffer = PILI_AMF_EncodeNumber(pBuffer, pBufEnd, prop->p_vu.p_number);
            break;

        case PILI_AMF_BOOLEAN:
            pBuffer = PILI_AMF_EncodeBoolean(pBuffer, pBufEnd, prop->p_vu.p_number != 0);
            break;

        case PILI_AMF_STRING:
            pBuffer = PILI_AMF_EncodeString(pBuffer, pBufEnd, &prop->p_vu.p_aval);
            break;

        case PILI_AMF_NULL:
            if (pBuffer + 1 >= pBufEnd)
                return NULL;
            *pBuffer++ = PILI_AMF_NULL;
            break;

        case PILI_AMF_OBJECT:
            pBuffer = PILI_AMF_Encode(&prop->p_vu.p_object, pBuffer, pBufEnd);
            break;

        default:
            PILI_RTMP_Log(PILI_RTMP_LOGERROR, "%s, invalid type. %d", __FUNCTION__, prop->p_type);
            pBuffer = NULL;
    };

    return pBuffer;
}

#define PILI_AMF3_INTEGER_MAX 268435455
#define PILI_AMF3_INTEGER_MIN -268435456

int PILI_AMF3ReadInteger(const char *data, int32_t *valp) {
    int i = 0;
    int32_t val = 0;

    while (i <= 2) { /* handle first 3 bytes */
        if (data[i] & 0x80) { /* byte used */
            val <<= 7; /* shift up */
            val |= (data[i] & 0x7f); /* add bits */
            i++;
        } else {
            break;
        }
    }

    if (i > 2) { /* use 4th byte, all 8bits */
        val <<= 8;
        val |= data[3];

        /* range check */
        if (val > PILI_AMF3_INTEGER_MAX)
            val -= (1 << 29);
    } else { /* use 7bits of last unparsed byte (0xxxxxxx) */
        val <<= 7;
        val |= data[i];
    }

    *valp = val;

    return i > 2 ? 4 : i + 1;
}

int PILI_AMF3ReadString(const char *data, PILI_AVal *str) {
    int32_t ref = 0;
    int len;
    assert(str != 0);

    len = PILI_AMF3ReadInteger(data, &ref);
    data += len;

    if ((ref & 0x1) == 0) { /* reference: 0xxx */
        uint32_t refIndex = (ref >> 1);
        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG,
                 "%s, string reference, index: %d, not supported, ignoring!",
                 __FUNCTION__, refIndex);
        return len;
    } else {
        uint32_t nSize = (ref >> 1);

        str->av_val = (char *)data;
        str->av_len = nSize;

        return len + nSize;
    }
    return len;
}

int PILI_AMF3Prop_Decode(PILI_AMFObjectProperty *prop, const char *pBuffer, int nSize,
                    int bDecodeName) {
    int nOriginalSize = nSize;
    PILI_AMF3DataType type;

    prop->p_name.av_len = 0;
    prop->p_name.av_val = NULL;

    if (nSize == 0 || !pBuffer) {
        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "empty buffer/no buffer pointer!");
        return -1;
    }

    /* decode name */
    if (bDecodeName) {
        PILI_AVal name;
        int nRes = PILI_AMF3ReadString(pBuffer, &name);

        if (name.av_len <= 0)
            return nRes;

        prop->p_name = name;
        pBuffer += nRes;
        nSize -= nRes;
    }

    /* decode */
    type = *pBuffer++;
    nSize--;

    switch (type) {
        case PILI_AMF3_UNDEFINED:
        case PILI_AMF3_NULL:
            prop->p_type = PILI_AMF_NULL;
            break;
        case PILI_AMF3_FALSE:
            prop->p_type = PILI_AMF_BOOLEAN;
            prop->p_vu.p_number = 0.0;
            break;
        case PILI_AMF3_TRUE:
            prop->p_type = PILI_AMF_BOOLEAN;
            prop->p_vu.p_number = 1.0;
            break;
        case PILI_AMF3_INTEGER: {
            int32_t res = 0;
            int len = PILI_AMF3ReadInteger(pBuffer, &res);
            prop->p_vu.p_number = (double)res;
            prop->p_type = PILI_AMF_NUMBER;
            nSize -= len;
            break;
        }
        case PILI_AMF3_DOUBLE:
            if (nSize < 8)
                return -1;
            prop->p_vu.p_number = PILI_AMF_DecodeNumber(pBuffer);
            prop->p_type = PILI_AMF_NUMBER;
            nSize -= 8;
            break;
        case PILI_AMF3_STRING:
        case PILI_AMF3_XML_DOC:
        case PILI_AMF3_XML: {
            int len = PILI_AMF3ReadString(pBuffer, &prop->p_vu.p_aval);
            prop->p_type = PILI_AMF_STRING;
            nSize -= len;
            break;
        }
        case PILI_AMF3_DATE: {
            int32_t res = 0;
            int len = PILI_AMF3ReadInteger(pBuffer, &res);

            nSize -= len;
            pBuffer += len;

            if ((res & 0x1) == 0) { /* reference */
                uint32_t nIndex = (res >> 1);
                PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "PILI_AMF3_DATE reference: %d, not supported!", nIndex);
            } else {
                if (nSize < 8)
                    return -1;

                prop->p_vu.p_number = PILI_AMF_DecodeNumber(pBuffer);
                nSize -= 8;
                prop->p_type = PILI_AMF_NUMBER;
            }
            break;
        }
        case PILI_AMF3_OBJECT: {
            int nRes = PILI_AMF3_Decode(&prop->p_vu.p_object, pBuffer, nSize, TRUE);
            if (nRes == -1)
                return -1;
            nSize -= nRes;
            prop->p_type = PILI_AMF_OBJECT;
            break;
        }
        case PILI_AMF3_ARRAY:
        case PILI_AMF3_BYTE_ARRAY:
        default:
            PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "%s - AMF3 unknown/unsupported datatype 0x%02x, @0x%08X",
                     __FUNCTION__, (unsigned char)(*pBuffer), pBuffer);
            return -1;
    }

    return nOriginalSize - nSize;
}

int PILI_AMFProp_Decode(PILI_AMFObjectProperty *prop, const char *pBuffer, int nSize,
                   int bDecodeName) {
    int nOriginalSize = nSize;
    int nRes;

    prop->p_name.av_len = 0;
    prop->p_name.av_val = NULL;

    if (nSize == 0 || !pBuffer) {
        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "%s: Empty buffer/no buffer pointer!", __FUNCTION__);
        return -1;
    }

    if (bDecodeName && nSize < 4) { /* at least name (length + at least 1 byte) and 1 byte of data */
        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG,
                 "%s: Not enough data for decoding with name, less than 4 bytes!",
                 __FUNCTION__);
        return -1;
    }

    if (bDecodeName) {
        unsigned short nNameSize = PILI_AMF_DecodeInt16(pBuffer);
        if (nNameSize > nSize - 2) {
            PILI_RTMP_Log(PILI_RTMP_LOGDEBUG,
                     "%s: Name size out of range: namesize (%d) > len (%d) - 2",
                     __FUNCTION__, nNameSize, nSize);
            return -1;
        }

        PILI_AMF_DecodeString(pBuffer, &prop->p_name);
        nSize -= 2 + nNameSize;
        pBuffer += 2 + nNameSize;
    }

    if (nSize == 0) {
        return -1;
    }

    nSize--;

    prop->p_type = *pBuffer++;
    switch (prop->p_type) {
        case PILI_AMF_NUMBER:
            if (nSize < 8)
                return -1;
            prop->p_vu.p_number = PILI_AMF_DecodeNumber(pBuffer);
            nSize -= 8;
            break;
        case PILI_AMF_BOOLEAN:
            if (nSize < 1)
                return -1;
            prop->p_vu.p_number = (double)PILI_AMF_DecodeBoolean(pBuffer);
            nSize--;
            break;
        case PILI_AMF_STRING: {
            unsigned short nStringSize = PILI_AMF_DecodeInt16(pBuffer);

            if (nSize < (long)nStringSize + 2)
                return -1;
            PILI_AMF_DecodeString(pBuffer, &prop->p_vu.p_aval);
            nSize -= (2 + nStringSize);
            break;
        }
        case PILI_AMF_OBJECT: {
            int nRes = PILI_AMF_Decode(&prop->p_vu.p_object, pBuffer, nSize, TRUE);
            if (nRes == -1)
                return -1;
            nSize -= nRes;
            break;
        }
        case PILI_AMF_MOVIECLIP: {
            PILI_RTMP_Log(PILI_RTMP_LOGERROR, "PILI_AMF_MOVIECLIP reserved!");
            return -1;
            break;
        }
        case PILI_AMF_NULL:
        case PILI_AMF_UNDEFINED:
        case PILI_AMF_UNSUPPORTED:
            prop->p_type = PILI_AMF_NULL;
            break;
        case PILI_AMF_REFERENCE: {
            PILI_RTMP_Log(PILI_RTMP_LOGERROR, "PILI_AMF_REFERENCE not supported!");
            return -1;
            break;
        }
        case PILI_AMF_ECMA_ARRAY: {
            nSize -= 4;

            /* next comes the rest, mixed array has a final 0x000009 mark and names, so its an object */
            nRes = PILI_AMF_Decode(&prop->p_vu.p_object, pBuffer + 4, nSize, TRUE);
            if (nRes == -1)
                return -1;
            nSize -= nRes;
            prop->p_type = PILI_AMF_OBJECT;
            break;
        }
        case PILI_AMF_OBJECT_END: {
            return -1;
            break;
        }
        case PILI_AMF_STRICT_ARRAY: {
            unsigned int nArrayLen = PILI_AMF_DecodeInt32(pBuffer);
            nSize -= 4;

            nRes = PILI_AMF_DecodeArray(&prop->p_vu.p_object, pBuffer + 4, nSize,
                                   nArrayLen, FALSE);
            if (nRes == -1)
                return -1;
            nSize -= nRes;
            prop->p_type = PILI_AMF_OBJECT;
            break;
        }
        case PILI_AMF_DATE: {
            PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "PILI_AMF_DATE");

            if (nSize < 10)
                return -1;

            prop->p_vu.p_number = PILI_AMF_DecodeNumber(pBuffer);
            prop->p_UTCoffset = PILI_AMF_DecodeInt16(pBuffer + 8);

            nSize -= 10;
            break;
        }
        case PILI_AMF_LONG_STRING: {
            unsigned int nStringSize = PILI_AMF_DecodeInt32(pBuffer);
            if (nSize < (long)nStringSize + 4)
                return -1;
            PILI_AMF_DecodeLongString(pBuffer, &prop->p_vu.p_aval);
            nSize -= (4 + nStringSize);
            prop->p_type = PILI_AMF_STRING;
            break;
        }
        case PILI_AMF_RECORDSET: {
            PILI_RTMP_Log(PILI_RTMP_LOGERROR, "PILI_AMF_RECORDSET reserved!");
            return -1;
            break;
        }
        case PILI_AMF_XML_DOC: {
            PILI_RTMP_Log(PILI_RTMP_LOGERROR, "PILI_AMF_XML_DOC not supported!");
            return -1;
            break;
        }
        case PILI_AMF_TYPED_OBJECT: {
            PILI_RTMP_Log(PILI_RTMP_LOGERROR, "PILI_AMF_TYPED_OBJECT not supported!");
            return -1;
            break;
        }
        case PILI_AMF_AVMPLUS: {
            int nRes = PILI_AMF3_Decode(&prop->p_vu.p_object, pBuffer, nSize, TRUE);
            if (nRes == -1)
                return -1;
            nSize -= nRes;
            prop->p_type = PILI_AMF_OBJECT;
            break;
        }
        default:
            PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "%s - unknown datatype 0x%02x, @0x%08X", __FUNCTION__,
                     prop->p_type, pBuffer - 1);
            return -1;
    }

    return nOriginalSize - nSize;
}

void PILI_AMFProp_Dump(PILI_AMFObjectProperty *prop) {
    char strRes[256];
    char str[256];
    PILI_AVal name;

    if (prop->p_type == PILI_AMF_INVALID) {
        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "Property: INVALID");
        return;
    }

    if (prop->p_type == PILI_AMF_NULL) {
        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "Property: NULL");
        return;
    }

    if (prop->p_name.av_len) {
        name = prop->p_name;
    } else {
        name.av_val = "no-name.";
        name.av_len = sizeof("no-name.") - 1;
    }
    if (name.av_len > 18)
        name.av_len = 18;

    snprintf(strRes, 255, "Name: %18.*s, ", name.av_len, name.av_val);

    if (prop->p_type == PILI_AMF_OBJECT) {
        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "Property: <%sOBJECT>", strRes);
        PILI_AMF_Dump(&prop->p_vu.p_object);
        return;
    }

    switch (prop->p_type) {
        case PILI_AMF_NUMBER:
            snprintf(str, 255, "NUMBER:\t%.2f", prop->p_vu.p_number);
            break;
        case PILI_AMF_BOOLEAN:
            snprintf(str, 255, "BOOLEAN:\t%s",
                     prop->p_vu.p_number != 0.0 ? "TRUE" : "FALSE");
            break;
        case PILI_AMF_STRING:
            snprintf(str, 255, "STRING:\t%.*s", prop->p_vu.p_aval.av_len,
                     prop->p_vu.p_aval.av_val);
            break;
        case PILI_AMF_DATE:
            snprintf(str, 255, "DATE:\ttimestamp: %.2f, UTC offset: %d",
                     prop->p_vu.p_number, prop->p_UTCoffset);
            break;
        default:
            snprintf(str, 255, "INVALID TYPE 0x%02x", (unsigned char)prop->p_type);
    }

    PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "Property: <%s%s>", strRes, str);
}

void PILI_AMFProp_Reset(PILI_AMFObjectProperty *prop) {
    if (prop->p_type == PILI_AMF_OBJECT)
        PILI_AMF_Reset(&prop->p_vu.p_object);
    else {
        prop->p_vu.p_aval.av_len = 0;
        prop->p_vu.p_aval.av_val = NULL;
    }
    prop->p_type = PILI_AMF_INVALID;
}

/* PILI_AMFObject */

char *
    PILI_AMF_Encode(PILI_AMFObject *obj, char *pBuffer, char *pBufEnd) {
    int i;

    if (pBuffer + 4 >= pBufEnd)
        return NULL;

    *pBuffer++ = PILI_AMF_OBJECT;

    for (i = 0; i < obj->o_num; i++) {
        char *res = PILI_AMFProp_Encode(&obj->o_props[i], pBuffer, pBufEnd);
        if (res == NULL) {
            PILI_RTMP_Log(PILI_RTMP_LOGERROR, "PILI_AMF_Encode - failed to encode property in index %d",
                     i);
            break;
        } else {
            pBuffer = res;
        }
    }

    if (pBuffer + 3 >= pBufEnd)
        return NULL; /* no room for the end marker */

    pBuffer = PILI_AMF_EncodeInt24(pBuffer, pBufEnd, PILI_AMF_OBJECT_END);

    return pBuffer;
}

int PILI_AMF_DecodeArray(PILI_AMFObject *obj, const char *pBuffer, int nSize,
                    int nArrayLen, int bDecodeName) {
    int nOriginalSize = nSize;
    int bError = FALSE;

    obj->o_num = 0;
    obj->o_props = NULL;
    while (nArrayLen > 0) {
        PILI_AMFObjectProperty prop;
        int nRes;
        nArrayLen--;

        nRes = PILI_AMFProp_Decode(&prop, pBuffer, nSize, bDecodeName);
        if (nRes == -1)
            bError = TRUE;
        else {
            nSize -= nRes;
            pBuffer += nRes;
            PILI_AMF_AddProp(obj, &prop);
        }
    }
    if (bError)
        return -1;

    return nOriginalSize - nSize;
}

int PILI_AMF3_Decode(PILI_AMFObject *obj, const char *pBuffer, int nSize, int bAMFData) {
    int nOriginalSize = nSize;
    int32_t ref;
    int len;

    obj->o_num = 0;
    obj->o_props = NULL;
    if (bAMFData) {
        if (*pBuffer != PILI_AMF3_OBJECT)
            PILI_RTMP_Log(PILI_RTMP_LOGERROR,
                     "AMF3 Object encapsulated in AMF stream does not start with PILI_AMF3_OBJECT!");
        pBuffer++;
        nSize--;
    }

    ref = 0;
    len = PILI_AMF3ReadInteger(pBuffer, &ref);
    pBuffer += len;
    nSize -= len;

    if ((ref & 1) == 0) { /* object reference, 0xxx */
        uint32_t objectIndex = (ref >> 1);

        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "Object reference, index: %d", objectIndex);
    } else /* object instance */
    {
        int32_t classRef = (ref >> 1);

        PILI_AMF3ClassDef cd = {{0, 0}};
        PILI_AMFObjectProperty prop;

        if ((classRef & 0x1) == 0) { /* class reference */
            uint32_t classIndex = (classRef >> 1);
            PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "Class reference: %d", classIndex);
        } else {
            int32_t classExtRef = (classRef >> 1);
            int i;

            cd.cd_externalizable = (classExtRef & 0x1) == 1;
            cd.cd_dynamic = ((classExtRef >> 1) & 0x1) == 1;

            cd.cd_num = classExtRef >> 2;

            /* class name */

            len = PILI_AMF3ReadString(pBuffer, &cd.cd_name);
            nSize -= len;
            pBuffer += len;

            /*std::string str = className; */

            PILI_RTMP_Log(PILI_RTMP_LOGDEBUG,
                     "Class name: %s, externalizable: %d, dynamic: %d, classMembers: %d",
                     cd.cd_name.av_val, cd.cd_externalizable, cd.cd_dynamic,
                     cd.cd_num);

            for (i = 0; i < cd.cd_num; i++) {
                PILI_AVal memberName;
                len = PILI_AMF3ReadString(pBuffer, &memberName);
                PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "Member: %s", memberName.av_val);
                PILI_AMF3CD_AddProp(&cd, &memberName);
                nSize -= len;
                pBuffer += len;
            }
        }

        /* add as referencable object */

        if (cd.cd_externalizable) {
            int nRes;
            PILI_AVal name = AVC("DEFAULT_ATTRIBUTE");

            PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "Externalizable, TODO check");

            nRes = PILI_AMF3Prop_Decode(&prop, pBuffer, nSize, FALSE);
            if (nRes == -1)
                PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "%s, failed to decode AMF3 property!",
                         __FUNCTION__);
            else {
                nSize -= nRes;
                pBuffer += nRes;
            }

            PILI_AMFProp_SetName(&prop, &name);
            PILI_AMF_AddProp(obj, &prop);
        } else {
            int nRes, i;
            for (i = 0; i < cd.cd_num; i++) /* non-dynamic */
            {
                nRes = PILI_AMF3Prop_Decode(&prop, pBuffer, nSize, FALSE);
                if (nRes == -1)
                    PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "%s, failed to decode AMF3 property!",
                             __FUNCTION__);

                PILI_AMFProp_SetName(&prop, PILI_AMF3CD_GetProp(&cd, i));
                PILI_AMF_AddProp(obj, &prop);

                pBuffer += nRes;
                nSize -= nRes;
            }
            if (cd.cd_dynamic) {
                int len = 0;

                do {
                    nRes = PILI_AMF3Prop_Decode(&prop, pBuffer, nSize, TRUE);
                    PILI_AMF_AddProp(obj, &prop);

                    pBuffer += nRes;
                    nSize -= nRes;

                    len = prop.p_name.av_len;
                } while (len > 0);
            }
        }
        PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "class object!");
    }
    return nOriginalSize - nSize;
}

int PILI_AMF_Decode(PILI_AMFObject *obj, const char *pBuffer, int nSize, int bDecodeName) {
    int nOriginalSize = nSize;
    int bError = FALSE; /* if there is an error while decoding - try to at least find the end mark PILI_AMF_OBJECT_END */

    obj->o_num = 0;
    obj->o_props = NULL;
    while (nSize > 0) {
        PILI_AMFObjectProperty prop;
        int nRes;

        if (nSize >= 3 && PILI_AMF_DecodeInt24(pBuffer) == PILI_AMF_OBJECT_END) {
            nSize -= 3;
            bError = FALSE;
            break;
        }

        if (bError) {
            PILI_RTMP_Log(PILI_RTMP_LOGERROR,
                     "DECODING ERROR, IGNORING BYTES UNTIL NEXT KNOWN PATTERN!");
            nSize--;
            pBuffer++;
            continue;
        }

        nRes = PILI_AMFProp_Decode(&prop, pBuffer, nSize, bDecodeName);
        if (nRes == -1)
            bError = TRUE;
        else {
            nSize -= nRes;
            pBuffer += nRes;
            PILI_AMF_AddProp(obj, &prop);
        }
    }

    if (bError)
        return -1;

    return nOriginalSize - nSize;
}

void PILI_AMF_AddProp(PILI_AMFObject *obj, const PILI_AMFObjectProperty *prop) {
    if (!(obj->o_num & 0x0f))
        obj->o_props =
            realloc(obj->o_props, (obj->o_num + 16) * sizeof(PILI_AMFObjectProperty));
    obj->o_props[obj->o_num++] = *prop;
}

int PILI_AMF_CountProp(PILI_AMFObject *obj) {
    return obj->o_num;
}

PILI_AMFObjectProperty *
    PILI_AMF_GetProp(PILI_AMFObject *obj, const PILI_AVal *name, int nIndex) {
    if (nIndex >= 0) {
        if (nIndex <= obj->o_num)
            return &obj->o_props[nIndex];
    } else {
        int n;
        for (n = 0; n < obj->o_num; n++) {
            if (AVMATCH(&obj->o_props[n].p_name, name))
                return &obj->o_props[n];
        }
    }

    return (PILI_AMFObjectProperty *)&AMFProp_Invalid;
}

void PILI_AMF_Dump(PILI_AMFObject *obj) {
    int n;
    PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "(object begin)");
    for (n = 0; n < obj->o_num; n++) {
        PILI_AMFProp_Dump(&obj->o_props[n]);
    }
    PILI_RTMP_Log(PILI_RTMP_LOGDEBUG, "(object end)");
}

void PILI_AMF_Reset(PILI_AMFObject *obj) {
    int n;
    for (n = 0; n < obj->o_num; n++) {
        PILI_AMFProp_Reset(&obj->o_props[n]);
    }
    free(obj->o_props);
    obj->o_props = NULL;
    obj->o_num = 0;
}

/* PILI_AMF3ClassDefinition */

void PILI_AMF3CD_AddProp(PILI_AMF3ClassDef *cd, PILI_AVal *prop) {
    if (!(cd->cd_num & 0x0f))
        cd->cd_props = realloc(cd->cd_props, (cd->cd_num + 16) * sizeof(PILI_AVal));
    cd->cd_props[cd->cd_num++] = *prop;
}

PILI_AVal *
    PILI_AMF3CD_GetProp(PILI_AMF3ClassDef *cd, int nIndex) {
    if (nIndex >= cd->cd_num)
        return (PILI_AVal *)&AV_empty;
    return &cd->cd_props[nIndex];
}
