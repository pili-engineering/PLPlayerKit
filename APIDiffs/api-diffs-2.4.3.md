# PLPlayerKit 2.4.2 to 2.4.3 API Differences

## General Headers

```
PLPlayer.h
```
- *Modified* `PLPlayerDelegate`
    - *Added* `- (void)player:(nonnull PLPlayer *)player willRenderFrame:(nullable CVPixelBufferRef)frame pts:(int64_t)pts sarNumerator:(int)sarNumerator sarDenominator:(int)sarDenominator;`
    - *Added* `- (nonnull AudioBufferList *)player:(nonnull PLPlayer *)player willAudioRenderBuffer:(nonnull AudioBufferList *)audioBufferList asbd:(AudioStreamBasicDescription)audioStreamDescription pts:(int64_t)pts sampleFormat:(PLPlayerAVSampleFormat)sampleFormat;`

    
- *Modified* `PLPlayer`
    - *Added* `@property (nonatomic, assign, readonly) NSTimeInterval connectTime;`
    - *Added* `@property (nonatomic, assign, readonly) NSTimeInterval firstVideoTime;`
