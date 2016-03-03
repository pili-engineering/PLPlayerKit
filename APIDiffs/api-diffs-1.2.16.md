# PLPlayerKit 1.2.15 to 1.2.16 API Differences

## General Headers

```
PLVideoPlayerController.h
```
- *Added* property `@property (nonatomic, assign) NSTimeInterval    timeout;`
- *Added* method `- (void)prepareToPlayWithCompletion:(void (^)(BOOL success))handler;`
- *Added* method `- (void)stop;`

```
PLAudioPlayerController.h
```
- *Modified* protocol `PLAudioPlayerControllerDelegate`
    - *Added* `- (void)audioPlayerControllerWillBeginBackgroundTask:(PLAudioPlayerController *)controller;`
    - *Added* `- (void)audioPlayerController:(PLAudioPlayerController *)controller willEndBackgroundTask:(BOOL)isExpirationOccured;`
- *Added* property `@property (nonatomic, assign, getter=isBackgroundPlayEnable) BOOL  backgroundPlayEnable;`
- *Added* property `@property (nonatomic, assign) NSTimeInterval    timeout;`
- *Added* method `- (void)prepareToPlayWithCompletion:(void (^)(BOOL success))handler;`
- *Added* method `- (void)stop;`
