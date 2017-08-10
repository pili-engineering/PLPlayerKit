# PLPlayerKit 2.4.3 to 3.0.0 API Differences

## General Headers

- *Deleted* `PLPlayerEnv.h`

```
PLPlayer.h
```
- *Modified* `PLPlayerDelegate`
    - *Deleted* `- (void)player:(nonnull PLPlayer *)player willRenderFrame:(nullable CVPixelBufferRef)frame;`
    - *Deleted* `- (nonnull AudioBufferList *)player:(nonnull PLPlayer *)player willAudioRenderBuffer:(nonnull AudioBufferList *)audioBufferList;`
    - *Deleted* `- (void)player:(nonnull PLPlayer *)player codecError:(nonnull NSError *)error;`

    
- *Modified* `PLPlayer`
    - *Deleted* `@property (nonatomic, strong) AVPlayer  * _Nullable avplayer;`
    - *Deleted* `@property (nonatomic, strong) AVPlayerItem * _Nullable avplayerItem;`
    - *Deleted* `@property (nonatomic, assign, readonly) int displayRatioWidth;`
    - *Deleted* `@property (nonatomic, assign, readonly) int displayRatioHeight;`
    - *Added* `@property (nonatomic, strong) NSString * _Nullable DRMKey;`
    - *Added* `@property (nonatomic, assign) double playSpeed;`

- *Modified* `NS_ENUM(NSInteger, PLPlayerStatus)`
    - *Added* `PLPlayerStatusCompleted`

```
PLPlayerError.h
```
- *Modified* `typedef NS_ENUM(NSInteger, PLPlayerError)`
    - *Added* `PLPlayerErrorHTTPErrorHTTPConnectFailed`

```
PLPlayerOption.h
```  
    - *Deleted* `extern NSString * _Nonnull PLPlayerOptionKeyVODFFmpegEnable;`
    - *Deleted* `extern NSString * _Nonnull PLPlayerOptionKeyProbeSize;`
    - *Added* `extern NSString * _Nullable PLPlayerOptionKeyVideoCacheFolderPath;`
