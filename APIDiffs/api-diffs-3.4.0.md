# PLPlayerKit 3.3.3 to 3.4.0 API Differences

## General Headers    

```
PLPlayer.h
```
- *Deprecated* `- (void)player:(nonnull PLPlayer *)player SEIData:(nullable NSData *)SEIData __deprecated_msg("Use player:SEIData:ts: instead");`


```
PLPlayerOption.h
```
- *Added* `extern NSString * _Nullable PLPlayerOptionKeyHeadUserAgent;
`
