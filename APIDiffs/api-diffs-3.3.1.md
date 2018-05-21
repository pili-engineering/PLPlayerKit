# PLPlayerKit 3.3.0 to 3.3.1 API Differences

## General Headers    
```
PLPlayer.h
```      

- *Delete* `PLPlayerStatusSeeking`

- *Delete* `PLPlayerStatusSeekFailed`

- *Add* `- (void)mp4PreLoadTime:(CMTime)loadTime;`

- *Add* `- (void)preStartPosTime:(CMTime)startTime;`

- *Changed* `- (void)player:(nonnull PLPlayer *)player loadedTimeRange:(CMTime)timeRange;`

- *Changed* `- (void)player:(nonnull PLPlayer *)player seekToCompleted:(BOOL)isCompleted;`

- *Changed* `- (void)preDNSHost:(nullable NSURL *)URL;`
