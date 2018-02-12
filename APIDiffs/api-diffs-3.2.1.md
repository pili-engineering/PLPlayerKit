# PLPlayerKit 3.2.0 to 3.2.1 API Differences

## General Headers    
```
PLPlayer.h
```      

- *Add* `PLPlayerStatusOpen`

- *Add* `typedef NS_ENUM(NSInteger, PLPlayerFirstRenderType) {
    PLPlayerFirstRenderTypeVideo = 0, // 视频
    PLPlayerFirstRenderTypeAudio // 音频
};`
- *Add* `@property (nonatomic, assign) CGRect videoClipFrame;`
- *Add* `@property (nonatomic, assign) BOOL loopPlay`

- *Add* `- (void)player:(nonnull PLPlayer *)player firstRender:(PLPlayerFirstRenderType)firstRenderType;`

- *Deprecated* `@property (nonatomic, assign, readonly) int rotate __deprecated_msg("Method deprecated in v3.2.1. Adjust automatically");`

- *Add* `- (BOOL)openPlayerWithURL:(nullable NSURL *)URL;`

- *Modify* `- (BOOL)play;`
- *Modify* `- (BOOL)playWithURL:(nullable NSURL *)URL __deprecated;`
- *Modify* `- (BOOL)playWithURL:(nullable NSURL *)URL sameSource:(BOOL)sameSource;`


```
PLPlayerOption.h
```      

- *Add* `extern NSString * _Nullable PLPlayerOptionKeyVideoCacheExtensionName;`
