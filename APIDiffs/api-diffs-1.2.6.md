# PLPlayerKit 1.2.5 to 1.2.6 API Differences

## General Headers

```
PLVideoPlayerController.h
```
- *Add* `@property (nonatomic, assign, getter=isMuted) BOOL  muted`;
- *Add* `@property (nonatomic, assign, readonly) CGFloat audioVolume;`
- *Add* `@property (nonatomic, assign, readonly) CGFloat duration;`
- *Add* `@property (nonatomic, assign, readonly) CGFloat position;`
- *Add* `- (void)forward;`
- *Add* `- (void)rewind;`
- *Add* `- (void)setMoviePosition:(CGFloat)position;`
