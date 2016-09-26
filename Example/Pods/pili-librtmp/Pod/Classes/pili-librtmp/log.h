/*
 *  Copyright (C) 2008-2009 Andrej Stepanchuk
 *  Copyright (C) 2009-2010 Howard Chu
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

#ifndef __RTMP_LOG_H__
#define __RTMP_LOG_H__

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
/* Enable this to get full debugging output */
/* #define _DEBUG */

#ifdef _DEBUG
#undef NODEBUG
#endif

typedef enum {
    PILI_RTMP_LOGCRIT = 0,
    PILI_RTMP_LOGERROR,
    PILI_RTMP_LOGWARNING,
    PILI_RTMP_LOGINFO,
    PILI_RTMP_LOGDEBUG,
    PILI_RTMP_LOGDEBUG2,
    PILI_RTMP_LOGALL
} PILI_RTMP_LogLevel;

extern PILI_RTMP_LogLevel PILI_RTMP_debuglevel;

typedef void(PILI_RTMP_LogCallback)(int level, const char *fmt, va_list);
void PILI_RTMP_LogSetCallback(PILI_RTMP_LogCallback *cb);
void PILI_RTMP_LogSetOutput(FILE *file);
void PILI_RTMP_LogPrintf(const char *format, ...);
void PILI_RTMP_LogStatus(const char *format, ...);
void PILI_RTMP_Log(int level, const char *format, ...);
void PILI_RTMP_LogHex(int level, const uint8_t *data, unsigned long len);
void PILI_RTMP_LogHexString(int level, const uint8_t *data, unsigned long len);
void PILI_RTMP_LogSetLevel(PILI_RTMP_LogLevel lvl);
PILI_RTMP_LogLevel PILI_RTMP_LogGetLevel(void);

#ifdef __cplusplus
}
#endif

#endif
