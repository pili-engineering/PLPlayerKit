# PLPlayerKit 2.2.2 to 2.2.3 API Differences

## General Headers

```
PLPlayer.h
```
- *Added* `extern NSString * _Nonnull playerVersion();`

- *Added* `typedef void (^ScreenShotWithCompletionHandler)(UIImage * _Nullable image);`

- *Modified* `PLPlayer`
    - *Added* `- (void)player:(nonnull PLPlayer *)player willRenderFrame:(nullable CVPixelBufferRef)frame;`
    - *Added* `- (void)setVolume:(float)volume;`
    - *Added* `- (float)getVolume;`
    - *Added* `- (void)getScreenShotWithCompletionHandler:(nullable ScreenShotWithCompletionHandler)handle;`   
