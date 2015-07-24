//
//  PLAudioManager.h
//  PLPlayerKit
//
//  Created on 15/7/24.
//  Copyright (c) 2015年 Pili Engineering. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface PLAudioManager : NSObject

+ (instancetype)sharedManager;

- (BOOL)activeAudioSession;
- (void)deactiveAudioSession;

@end
