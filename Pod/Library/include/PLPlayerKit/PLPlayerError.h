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
    PLPlayerErrorUnknow = 0,  // "Unknow error"
    
    // Input error
    PLPlayerErrorEOF = -1, // End of file
    
    PLPlayerErrorURLNotSupported = -2000,  // "URL to play is not supported."
    
    PLPlayerErrorAudioSessionNotSupportToPlay = -2001,  // "AVAudioSession's category doesn't support audio play."
    
    PLPlayerErrorAudioFormatNotSupport = -2002, // "RTMP/FLV live audio only support AAC."
    
    PLPlayerErrorVideoFormatNotSupport = -2003, // "RTMP/FLV live video only support H264."
    
    PLPlayerErrorStreamFormatNotSupport = -2004, // FFMPEG can not open stream, or can not find stream info.'
    
    PLPlayerErrorInputTimeout = -2100, // "Input read data timeout."
    
    PLPLayerErrorInputReadError = -2101, // "Input read data error."
    
    // Codec error
    PLPlayerErrorCodecInitFailed = -2201, // "codec init failed."
    
    PLPlayerErrorHWCodecInitFailed = -2202, // "hardware codec init faile."
    
    PLPlayerErrorDecodeFailed = -2203,   // "decode failed."
    
    PLPlayerErrorHWDecodeFailed = -2204, // "hardware decode failed."
    
    PLPlayerErrorDecodeNoFrame = -2205, // "decode no frame."
    
    PLPlayerErrorVideoSizeChange = -2206, // "video size change, should stop and replay."
    
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
    
    PLPlayerErrorHTTPErrorHTTPConnectFailed = -1202, // "HTTP connect failed"
};



#endif /* PLPlayerError_h */
