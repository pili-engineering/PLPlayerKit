# PLPlayerKit 2.2.1 to 2.2.2 API Differences

## General Headers

```
PLPlayer.h
```
- *Modified* `PLPlayer`
    - *Added* `@property (nonatomic,assign,getter = isAutoReconnectEnable) BOOL autoReconnectEnable;`
- *Modified* `NS_ENUM(NSInteger, PLPlayerStatus)`
	- *Added* `PLPlayerStateAutoReconnecting`    

