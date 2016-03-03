# PLPlayerKit 1.2.7 to 1.2.8 API Differences

## General Headers

```
PLVideoPlayerController.h
```
- *Add* method ```- (void)videoPlayerController:(PLVideoPlayerController *)playerController positionDidChange:(NSTimeInterval)position;```
- *Modified* property type ```duration``` from ```CGFloat``` to ```NSTimeInterval```
- *Modified* property type ```position``` from ```CGFloat``` to ```NSTimeInterval```
- *Modified* method ```- (void)setMoviePosition:(CGFloat)position;``` to ```- (void)setMoviePosition:(NSTimeInterval)position;```
