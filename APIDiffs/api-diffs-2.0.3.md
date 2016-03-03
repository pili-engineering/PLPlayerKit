# PLPlayerKit 2.0.2 to 2.0.3 API Differences

## General Headers

```
PLPlayer.h
```

- *Modified* `PLPlayer`
    - *Added* `@property (nonatomic, assign, getter=isBackgroundPlayEnable) BOOL backgroundPlayEnable;`
- *Modified* `PLPlayerDelegate`
    - *Added* `- (void)playerWillBeginBackgroundTask:(nonnull PLPlayer *)player;`
    - *Added* `- (void)player:(nonnull PLPlayer *)player willEndBackgroundTask:(BOOL)isExpirationOccured;`
- *Added* `AVAudioSessionAvailabilityCheck`
