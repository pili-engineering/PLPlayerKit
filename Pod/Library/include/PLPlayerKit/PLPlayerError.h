//
//  PLPlayerError.h
//  PLPlayerKit
//
//  Created by liang on 4/22/16.
//  Copyright © 2016年 Pili Engineering. All rights reserved.
//

#ifndef PLPlayerError_h
#define PLPlayerError_h

typedef NS_ENUM(NSInteger, PLPlayerError) {
    
    // PLPlayer error
    PLPlayerErrorUnknow = -1,  // "Unknow error"
    
    PLPlayerErrorURLNotSupported = -2000,  // "URL to play is not supported."
    
    PLPlayerErrorAudioSessionNotSupportToPlay = -2001,   // "AVAudioSession's category doesn't support audio play."
    
    // RTMP error
    PLPlayerErrorRTMPErrorUnknowOption = -999, // "Unknown option %s"
    
    PLPlayerErrorRTMPErrorAccessDNSFailed = -1000,	// "Failed to access the DNS. (addr: %s)"
    
    PLPlayerErrorRTMPErrorFailedToConnectSocket = -1001, // "Failed to connect socket. %d (%s)"
    
    PLPlayerErrorRTMPErrorSocksNegotiationFailed = -1002, // "Socks negotiation failed"
    
    PLPlayerErrorRTMPErrorFailedToCreateSocket = -1003, // "Failed to create socket. %d (%s)"
    
    PLPlayerErrorRTMPErrorHandshakeFailed = -1004,	// "Handshake failed"
    
    PLPlayerErrorRTMPErrorRTMPConnectFailed = -1005, // "RTMP connect failed"
    
    PLPlayerErrorRTMPErrorSendFailed = -1006, // "Send error %d (%s), (%d bytes)"
    
    PLPlayerErrorRTMPErrorServerRequestedClose = -1007, //	"RTMP server requested close"
    
    PLPlayerErrorRTMPErrorNetStreamFailed = -1008, // "NetStream failed"
    
    PLPlayerErrorRTMPErrorNetStreamPlayFailed = -1009, // "NetStream play failed"
    
    PLPlayerErrorRTMPErrorNetStreamPlayStreamNotFound = -1010, // "NetStream play stream not found"
    
    PLPlayerErrorRTMPErrorNetConnectionConnectInvalidApp = -1011, // "NetConnection connect invalip app"
    
    PLPlayerErrorRTMPErrorSanityFailed = -1012, //	"Sanity failed. Trying to send header of type: 0x%02X"
    
    PLPlayerErrorRTMPErrorSocketClosedByPeer = -1013, // "RTMP socket closed by peer"
    
    PLPlayerErrorRTMPErrorRTMPConnectStreamFailed = -1014,	// "RTMP connect stream failed"
    
    PLPlayerErrorRTMPErrorSocketTimeout = -1015, // "RTMP socket timeout"
    
    // SSL errors
    PLPlayerErrorRTMPErrorTLSConnectFailed = -1200,	//	"TLS_Connect failed"
    
    PLPlayerErrorRTMPErrorNoSSLOrTLSSupport = -1201,	//	"No SSL/TLS support"
};



#endif /* PLPlayerError_h */
