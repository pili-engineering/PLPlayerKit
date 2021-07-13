# PLPlayerKit 3.4.5 to 3.4.6 API Differences

## General Headers    

- *Modified*  `PLPlayerOption.h`

  - *Added*   `extern NSString * _Nullable PLPlayerOptionKeyVideoFileNameEncode`

- *Modified* `PLPlayer`

  - *Added*  `@property (nonatomic, assign, readonly) int audioFPS`

  - *Added*  `@property (nonatomic, assign, readonly) double audioBitrate`

  - *Added* - (**void**)addCacheUrl:(NSString ***_Nullable**)url;

  - *Added* - (**void**)deleteCacheUrl:(NSString ***_Nullable**)url;

  - *Added* - (**void**)addIOCache:(NSString ***_Nullable**)url;

  - *Added* - (**void**)deleteIOCache:(NSString ***_Nullable**)url;

  - *Added* - (**void**)setIOCacheSize:(NSInteger)size;

    