//
//  PLPlayerOption.h
//  PLPlayerKit
//
//  Created by 何昊宇 on 2017/5/15.
//  Copyright © 2017年 Aaron. All rights reserved.
//

#import <Foundation/Foundation.h>

/**
 设置控制台 log 的级别，log 级别在 kPLLogInfo 及以上，则会缓存日志文件，
 日志文件存放的位置为 APP Container/Library/Caches/Pili/PlayerLogs
 
 */
typedef enum : NSUInteger {
    /**
     *  No logs
     */
    kPLLogNone,
    /**
     *  Error logs only
     */
    kPLLogError,
    /**
     *  Error and warning logs
     */
    kPLLogWarning,
    /**
     *  Error, warning and info logs
     */
    kPLLogInfo,
    /**
     *  Error, warning, info and debug logs
     */
    kPLLogDebug,
    /**
     *  Error, warning, info, debug and verbose logs
     */
    kPLLogVerbose,
} PLLogLevel;

/**
 预设置播放器播放 URL 类型
 */
typedef enum : NSUInteger {
    /**
     * 未知类型
     */
    kPLPLAY_FORMAT_UnKnown,
    /**
     * M3U8 类型
     */
    kPLPLAY_FORMAT_M3U8,
    /**
     * MP4 类型
     */
    kPLPLAY_FORMAT_MP4,
    /**
     *  FLV 类型
     */
    kPLPLAY_FORMAT_FLV,
    /**
     *  MP3 类型,此类型需要提前设置，才可播放
     */
    kPLPLAY_FORMAT_MP3,
    /**
     *  AAC 类型,此类型需要提前设置，才可播放
     */
    kPLPLAY_FORMAT_AAC
    
} PLPlayFormat;


/**
   回调输出格式
   仅支持iOS端常见视频模式
 */
typedef enum : NSUInteger {
    /**
     * 默认nv12  即 kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange
     */
    kPLPlayOutputFormatYUV420,

    /**
     * BGRA
     */
    kPLPlayOutputFormatBGRA,

} PLPlayOutputFormat;

/**
 @abstract 接收/发送数据包超时时间间隔所对应的键值，单位为 s ，默认配置为 10s
 
 @warning 建议设置正数。设置的值小于等于 0 时，表示禁用超时，播放卡住时，将无超时回调。
 该参数仅对 rtmp/flv 直播生效
 
 @since v1.0.0
 */
extern NSString  * _Nonnull PLPlayerOptionKeyTimeoutIntervalForMediaPackets;

/**
 @abstract 一级缓存大小，单位为 ms，默认为 2000ms，增大该值可以减小播放过程中的卡顿率，但会增大弱网环境的最大累积延迟
 
 @discussion 该缓存存放的是网络层读取到的数据，为保证实时性，超过该缓存池大小的过期音频数据将被丢弃，视频将加速渲染追上音频
 
 
 @since v2.1.3
 */
extern NSString  * _Nonnull PLPlayerOptionKeyMaxL1BufferDuration;

/**
 @abstract 默认二级缓存大小，单位为 ms，默认为 300ms，增大该值可以减小播放过程中的卡顿率，但会增大弱网环境的最大累积延迟
 
 @discussion 该缓存存放的是解码之后待渲染的数据，如果该缓存池满，则二级缓存将不再接收来自一级缓存的数据
 
 
 @since v2.1.3
 */
extern NSString  * _Nonnull PLPlayerOptionKeyMaxL2BufferDuration;


/**
 @abstract 用于是否根据最小缓冲时间和最大缓冲时间，使播放速度变慢或变快
 
 @discussion 默认 YES ，即底层会自动根据缓存 buffer 调节速率。注意点播播放的时候，需关闭这个参数
  
 @since v3.4.7
 */
extern NSString  * _Nonnull PLPlayerOptionKeyCacheBufferDurationSpeedAdjust;


/**
 @abstract 是否使用 video toolbox 硬解码。
 
 @discussion 使用 video toolbox Player 将尝试硬解码，失败后，将切换回软解码。
 
 @waring 该参数仅对 rtmp/flv 直播生效, 默认不使用。支持 iOS 8.0 及更高版本。
 
 @since v2.1.4
 */
extern NSString  * _Nonnull PLPlayerOptionKeyVideoToolbox;

/**
 @abstract 配置 log 级别
 
 @discussion release 建议使用 kPLLogWarning, debug 建议使用 kPLLogInfo.
 
 @waring 取值范围: PLLogLevel
 
 @since v2.2.1
 */
extern NSString * _Nonnull PLPlayerOptionKeyLogLevel;

/**
 @abstract dns 查询，使用内置 DNS 解析
 
 @discussion NSString 类型，开启 DNS 解析，默认使用系统 API 解析
 
 @waring 该参数仅对 rtmp/flv 直播生效
 
 @since v2.2.1
 */
extern NSString * _Nonnull PLPlayerOptionKeyDNSManager;

/**
 @abstract 开启/关闭 HappyDNS 的 DNS 解析
 
 @discussion 默认关闭
 
 @waring 该参数仅对 rtmp 与 flv 生效，值类型为 BOOL
 
 @since v2.3.0
 */
extern NSString * _Nonnull PLPlayerOptionKeyHappyDNSEnable __deprecated_msg("Method deprecated in v3.3.0. No support HappyDNS");

/**
 @abstract 视频缓存目录, 默认为 nil
 
 @waring 该属性仅对点播 mp4 有效, 当 PLPlayerOptionKeyVideoCacheFolderPath 有值时，默认关闭 DNS  解析
 
 @since v3.0.0
 */
extern NSString * _Nullable PLPlayerOptionKeyVideoCacheFolderPath;


/**
 @abstract 视频缓存目录名称是否加密，默认不加密
 
 @waring 当 PLPlayerOptionKeyVideoCacheFolderPath 有值时，成效
 
 @since v3.4.6
 */
extern NSString * _Nullable PLPlayerOptionKeyVideoFileNameEncode;

/**
 @abstract 视频预设值播放 URL 格式类型
 
 @discussion 该参数用于加快首开，提前告知播放器流类型，默认 kPLPLAY_FORMAT_UnKnown
 
 @waring 取值范围: PLPlayFormat
 
 @since v3.0.1
 */
extern NSString * _Nullable PLPlayerOptionKeyVideoPreferFormat;

/**
 @abstract 视频缓存扩展名
 
 @waring 该属性仅对点播 mp4 有效，需在视频缓存目录 PLPlayerOptionKeyVideoCacheFolderPath 基础上设置
 
 @see PLPlayerOptionKeyVideoCacheFolderPath
 
 @since v3.2.1
 */
extern NSString * _Nullable PLPlayerOptionKeyVideoCacheExtensionName;

/**
 SDK 设备 ID
 
 @discussion 该参数用于标记 SDK 设备 ID, 默认为 UUID
 
 @since v3.0.2
 */
extern NSString * _Nullable PLPlayerOptionKeySDKID;

/**
 http 的 header
 
 @discussion 该参数用于设置 http 的 header
 
 @warning 不可包含 "\n" 或 "\r"，包含"\n" 或 "\r" 则设置无效
 
 @since v3.4.0
 */
extern NSString * _Nullable PLPlayerOptionKeyHeadUserAgent;

/**
  视频回调格式
 
 @discussion 该参数用于自定义渲染，设置播放器解码回调数据格式
 
 @since v3.4.5
 */
extern NSString * _Nullable PLPlayerOptionKeyVideoOutputFormat;
/**
 PLPlayer 的可选配置类，在初始化 PLPlayer 对象的时候传入其实例进行 PLPlayer 的可选项配置
 
 @since v1.0.0
 */
@interface PLPlayerOption : NSObject

/**
 @abstract 使用默认配置生成一个 PLPlayerOption 对象
 
 @return 生成的 PLPlayerOption 对象
 
 @since v1.0.0
 */
+ (nonnull PLPlayerOption *)defaultOption;

/**
 使用一个 NSDictionary 对象来生成一个 PLPlayerOption 对象
 
 @param dic 用于初始化 PLPlyerOption 对象的 NSDictionary 对象
 
 @return 生成的 PLPlayerOption 对象
 
 @since v1.0.0
 */
+ (nonnull PLPlayerOption *)optionWithDictionary:(nonnull NSDictionary *)dic;

/**
 设置相应的键所对应的值
 
 @param value 要设置的值
 @param key   要设置值的键
 
 @since v1.0.0
 */
- (void)setOptionValue:(nullable id)value forKey:(nonnull NSString *)key;

/**
 获取特定键所对应的值
 
 @param key 要获取值的键名
 
 @return 获取到的值，当该值为 nil 时表示没有找到该键名对应的值。
 
 @since v1.0.0
 */
- (nullable id)optionValueForKey:(nonnull NSString *)key;

/**
 生成一个包含该 PLPlayer 所有键值对信息的 NSDictionary 对象来
 
 @return 所生成的 NSDictionary 对象
 
 @since v1.0.0
 */
- (nonnull NSDictionary *)dictionaryValue;


@end
