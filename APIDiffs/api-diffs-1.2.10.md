# PLPlayerKit 1.2.9 to 1.2.10 API Differences

## General Headers

```
PLPlayerTypeDefines.h
```
- *Add* notifications
```
extern NSString *PLAudioSessionInterruptionStateKey;
extern NSString *PLAudioSessionInterruptionTypeKey;
extern NSString *PLAudioSessionDidInterrupteNotification;
extern NSString *PLAudioSessionRouteChangeReasonKey;
extern NSString *PLAudioSessionRouteDidChangeNotification;
extern NSString *PLAudioSessionCurrentHardwareOutputVolumeKey;
extern NSString *PLAudioSessionCurrentHardwareOutputVolumeDidChangeNotification;
```
