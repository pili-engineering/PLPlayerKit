#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdlib.h>

typedef struct RTMPError {
    int code;
    char *message;
} RTMPError;

void PILI_RTMPError_Alloc(RTMPError *error, size_t msg_size);
void PILI_RTMPError_Free(RTMPError *error);
void PILI_RTMPError_Message(RTMPError *error, int code, const char *message);

// error defines
enum {
    PILI_RTMPErrorUnknow = -1, //	"Unknow error"
    PILI_RTMPErrorUnknowOption = -999, //	"Unknown option %s"
    PILI_RTMPErrorAccessDNSFailed = -1000, //	"Failed to access the DNS. (addr: %s)"
    PILI_RTMPErrorFailedToConnectSocket =
        -1001, //	"Failed to connect socket. %d (%s)"
    PILI_RTMPErrorSocksNegotiationFailed = -1002, //	"Socks negotiation failed"
    PILI_RTMPErrorFailedToCreateSocket =
        -1003, //	"Failed to create socket. %d (%s)"
    PILI_RTMPErrorHandshakeFailed = -1004, //	"Handshake failed"
    PILI_RTMPErrorRTMPConnectFailed = -1005, //	"RTMP connect failed"
    PILI_RTMPErrorSendFailed = -1006, //	"Send error %d (%s), (%d bytes)"
    PILI_RTMPErrorServerRequestedClose = -1007, //	"RTMP server requested close"
    PILI_RTMPErrorNetStreamFailed = -1008, //	"NetStream failed"
    PILI_RTMPErrorNetStreamPlayFailed = -1009, //	"NetStream play failed"
    PILI_RTMPErrorNetStreamPlayStreamNotFound =
        -1010, //	"NetStream play stream not found"
    PILI_RTMPErrorNetConnectionConnectInvalidApp =
        -1011, //	"NetConnection connect invalip app"
    PILI_RTMPErrorSanityFailed =
        -1012, //	"Sanity failed. Trying to send header of type: 0x%02X"
    PILI_RTMPErrorSocketClosedByPeer = -1013, // "RTMP socket closed by peer"
    PILI_RTMPErrorRTMPConnectStreamFailed = -1014, // "RTMP connect stream failed"
    PILI_RTMPErrorSocketTimeout = -1015, // "RTMP socket timeout"

    // SSL errors
    PILI_RTMPErrorTLSConnectFailed = -1200, //	"TLS_Connect failed"
    PILI_RTMPErrorNoSSLOrTLSSupport = -1201, //	"No SSL/TLS support"
};

#endif
