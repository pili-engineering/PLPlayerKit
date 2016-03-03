# PLPlayerKit 2.0.4 to 2.1.0 API Differences

## General Headers

- *Added* Class `PLPlayerOption`
- *Added* Header `PLPlayerOption.h`

```
PLPlayerOption.h
```
- *Added* `extern NSString  * _Nonnull PLPlayerOptionKeyTimeoutIntervalForMediaPackets;`
- *Added* `+ (nonnull PLPlayerOption *)defaultOption;`
- *Added* `+ (nonnull PLPlayerOption *)optionWithDictionary:(nonnull NSDictionary *)dic;`
- *Added* `- (void)setOptionValue:(nullable id)value forKey:(nonnull NSString *)key;`
- *Added* `- (nullable id)optionValueForKey:(nonnull NSString *)key;`
- *Added* `- (nonnull NSDictionary *)dictionaryValue;`

```
PLPlayer.h
```
- *Added* `@property (nonatomic, strong, nullable) dispatch_queue_t delegateQueue;`
- *Added* `@property (nonnull, strong, readonly)   PLPlayerOption * option;`
- *Modified* `@property (nonatomic, strong, nonnull) NSURL * url;` to `@property (nonatomic, copy, nonnull, readonly) NSURL * URL;`
- *Modified* `+ (nullable instancetype)playerWithURL:(nullable NSURL *)url;` to `+ (nullable instancetype)playerWithURL:(nullable NSURL *)URL option:(nullable PLPlayerOption *)option;`
- *Modified* `- (nullable instancetype)initWithURL:(nullable NSURL *)url;` to `- (nullable instancetype)initWithURL:(nullable NSURL *)URL option:(nullable PLPlayerOption *)option;`
- *Deleted* `@property (nonatomic, strong, nullable) NSError * error;`
- *Deleted* `@property (nonatomic, assign) NSTimeInterval timeoutIntervalForMediaPackets;`
- *Deleted* `- (void)prepareToPlayWithCompletion:(nullable void (^)(NSError * _Nullable error))completion;`
