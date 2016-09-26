# PLPlayerKit 2.2.4 to 2.3.0 API Differences

## General Headers

```
PLPlayer.h
```
- *Added* `typedef NS_ENUM(NSInteger, PLPLayerRotationsMode)`

- *Modified* `PLPlayer`
    - *Added* `@property (nonatomic, assign) PLPLayerRotationsMode rotationMode;`
    - *Added* `@property (nonatomic, assign) BOOL enableRender;`
    - *Added* `@property (nonatomic, assign, readonly) int width;`
    - *Added* `@property (nonatomic, assign, readonly) int height;`
    - *Added* `@property (nonatomic, assign, readonly) int videoFPS;`
    - *Added* `@property (nonatomic, assign, readonly) int renderFPS;`
    - *Added* `@property (nonatomic, assign, readonly) double bitrate;`
    - *Added* `@property (nonatomic, assign, readonly) double downSpeed;`

```
PLPlayerOption.h
```  
- *Added* `extern NSString * _Nonnull PLPlayerOptionKeyHappyDNSEnable;`
- *Added* `extern NSString * _Nonnull PLPlayerOptionKeyVODFFmpegEnable;`

```
PLPlayerError.h
```
- *Modified* `typedef NS_ENUM(NSInteger, PLPlayerError)`
    - *Added* `PLPlayerErrorAudioFormatNotSupport`
    - *Added* `PLPlayerErrorVideoFormatNotSupport`
    - *Added* `PLPlayerErrorStreamFormatNotSupport`
    - *Added* `PLPlayerErrorInputTimeout`
    - *Added* `PLPLayerErrorInputReadError`
