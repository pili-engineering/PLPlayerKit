# PLPlayerKit 2.3.0 to 2.4.0 API Differences

## General Headers

```
PLPlayer.h
```
- *Modified* `PLPlayerDelegate`
    - *Added* `- (void)player:(nonnull PLPlayer *)player codecError:(nonnull NSError *)error;`
    
- *Modified* `PLPlayer`
    - *Added* `+ (void)preDNSHost:(nullable NSURL *)URL;`
    - *Added* `@property (nonatomic, strong, nullable, readonly) UIImageView *launchView;`
    - *Added* `@property (nonatomic, strong, readonly) NSDictionary * _Nullable metadata;`
    - *Added* `@property (nonatomic, assign, readonly) int displayRatioWidth;`
    - *Added* `@property (nonatomic, assign, readonly) int displayRatioHeight;`

```
PLPlayerError.h
```
- *Modified* `typedef NS_ENUM(NSInteger, PLPlayerError)`
    - *Added* `PLPlayerErrorCodecInitFailed`
    - *Added* `PLPlayerErrorHWCodecInitFailed`
    - *Added* `PLPlayerErrorDecodeFailed`
    - *Added* `PLPlayerErrorHWDecodeFailed`
