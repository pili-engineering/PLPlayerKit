# PLPlayerKit 2.1.0 to 2.1.1 API Differences

## General Headers

```
PLPlayer.h
```
- *Modified* `- (void)player:(nonnull PLPlayer *)player willEndBackgroundTask:(BOOL)isExpirationOccured;		` to `- (void)playerWillEndBackgroundTask:(nonnull PLPlayer *)player;`
