# PLPlayerKit 2.1.1 to 2.1.2 API Differences

## General Headers

- *Added* Header `PLPlayerError.h`

```
PLPlayerError.h
```
- *Added* `NS_ENUM(NSInteger, PLPlayerError)`

```
PLPlayer.h
```
- *Added* `@property (nonatomic, assign, getter=isMute) BOOL mute;`
- *Added* `@property (nonatomic, assign, readonly) CMTime  currentTime;`
- *Added* `@property (nonatomic, assign, readonly) CMTime  totalDuration;`
- *Added* `- (void)seekTo:(CMTime)time;`
