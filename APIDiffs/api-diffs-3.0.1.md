# PLPlayerKit 3.0.0 to 3.0.1 API Differences

## General Headers

```
PLPlayer.h
```
- *Modified* `PLPlayerDelegate`
    - *Added* `- (void)player:(nonnull PLPlayer *)player SEIData:(nullable NSData *)SEIData;`
    - *Added* `- (void)playWithURL:(nullable NSURL *)URL sameSource:(BOOL)sameSource;`
    - *Deprecated* `- (void)playWithURL:(nullable NSURL *)URL`
    
```
PLPlayerOption.h
```  
- *Added* `extern NSString * _Nullable PLPlayerOptionKeyVideoPreferFormat;`

