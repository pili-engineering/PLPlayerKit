# PLPlayerKit 3.2.1 to 3.3.0 API Differences

## General Headers    
```
PLPlayer.h
```      

- *Add* `PLPlayerStatusSeeking`

- *Add* `PLPlayerStatusSeekFailed`

- *Add* `- (long long)getHttpBufferSize;`

- *Add* `- (void)setBufferingEnabled:(BOOL)bufferingEnabled;`

- *Add* `- (BOOL)getBufferingEnabled;`

- *Add* `- (void)player:(nonnull PLPlayer *)player width:(int)width height:(int)height;`

- *Add* `- (void)playerSeekToCompleted:(nonnull PLPlayer *)player;`


```
PLPlayerOption.h
```      

- *Deprecated* `extern NSString * _Nonnull PLPlayerOptionKeyHappyDNSEnable __deprecated_msg("Method deprecated in v3.3.0 No support HappyDNS");`

