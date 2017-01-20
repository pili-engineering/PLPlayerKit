# PLPlayerKit 2.4.0 to 2.4.1 API Differences

## General Headers

```
PLPlayer.h
```
- *Modified* `PLPlayerDelegate`
    - *Added* `- (void)player:(nonnull PLPlayer *)player loadedTimeRange:(CMTimeRange)timeRange;`
    
- *Modified* `PLPlayer`
    - *Added* `@property (nonatomic, strong) AVPlayer  * _Nullable avplayer;`
    - *Added* `@property (nonatomic, strong) AVPlayerItem * _Nullable avplayerItem;`
    - *Added* `@property (nonatomic, strong) NSString * _Nonnull referer;`
    - *Added* `- (void)playWithURL:(nullable NSURL *)URL;`

