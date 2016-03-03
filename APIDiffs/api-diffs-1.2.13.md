# PLPlayerKit 1.2.12 to 1.2.13 API Differences

## General Headers

```
PLPlayerKit.h
```
- *Added* `#import "PLAudioPlayerController.h"`
`PLAudioPlayerController.h`
- *Added* Class `PLAudioPlayerController`

```
PLVideoPlayerController.h
```
- *Added* `- (void)seekTo:(NSTimeInterval)position;`
- *Deprecated* `- (void)setMoviePosition:(NSTimeInterval)position;`

```
PLPlayerTypeDefines.h
```
- *Added* `PLPlayerParameterMinBufferedDuration`
- *Added* `PLPlayerParameterMaxBufferedDuration`
- *Added* `PLPlayerParameterAutoPlayEnable`
- *Added* `PLVideoParameterDisableDeinterlacing`
- *Added* `PLVideoParameterFrameViewContentMode`
- *Deprecated* `PLMovieParameterMinBufferedDuration`
- *Deprecated* `PLMovieParameterMaxBufferedDuration`
- *Deprecated* `PLMovieParameterDisableDeinterlacing`
- *Deprecated* `PLMovieParameterFrameViewContentMode`
- *Deprecated* `PLMovieParameterAutoPlayEnable`
- *Added* `PLPlayerState`
- *Deprecated* `PLVideoPlayerState`
