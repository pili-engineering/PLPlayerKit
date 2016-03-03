# PLPlayerKit 1.2.6 to 1.2.7 API Differences

## General Headers

```
PLPlayerTypeDefines.h
```
- *Add* type `PLVideoPlayerState`
- *Add* const `extern NSString * const PLMovidParameterAutoPlayEnable;`

```
PLVideoPlayerController.h
```
- *Modified* protocol ```PLVideoPlayerControllerDelegate```
	- *Add* method ```- (void)videoPlayerController:(PLVideoPlayerController *)controller playerStateDidChange:(PLVideoPlayerState)state;```
	- *Add* method ```- (void)videoPlayerControllerDecoderHasBeenReady:(PLVideoPlayerController *)controller;```
- *Add* property ```@property (nonatomic, assign, readonly) PLVideoPlayerState playerState;```
