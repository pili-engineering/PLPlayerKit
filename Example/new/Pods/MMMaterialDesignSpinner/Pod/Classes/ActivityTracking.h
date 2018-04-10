/*
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
 */

#import <UIKit/UIKit.h>

@protocol ActivityTracking

// default is UIActivityIndicatorViewStyleWhite
@property(nonatomic) UIActivityIndicatorViewStyle activityIndicatorViewStyle;

// default is YES. calls -setHidden when animating gets set to NO
@property(nonatomic) BOOL hidesWhenStopped;

@property (nullable, readwrite, nonatomic, strong) UIColor *color NS_AVAILABLE_IOS(5_0) UI_APPEARANCE_SELECTOR;

/** Sets the line width of the spinner's circle. */
@property (nonatomic) CGFloat lineWidth;

/** Sets the line cap of the spinner's circle. */
@property (nonatomic, strong) NSString * _Nullable lineCap;

/** Specifies the timing function to use for the control's animation. Defaults to kCAMediaTimingFunctionEaseInEaseOut */
@property (nullable, nonatomic, strong) CAMediaTimingFunction *timingFunction;

/** Property indicating the duration of the animation, default is 1.5s. Should be set prior to -[startAnimating] */
@property (nonatomic, readwrite) NSTimeInterval duration;

/** Property to manually set the percent complete of the spinner, in case you don't want to start at 0. Valid values are 0.0 to 1.0 */
@property (nonatomic) CGFloat percentComplete;

/**
 *  Convenience function for starting & stopping animation with a boolean variable instead of explicit
 *  method calls.
 *
 *  @param animate true to start animating, false to stop animating.
 @note This method simply calls the startAnimating or stopAnimating methods based on the value of the animate parameter.
 */
- (void)setAnimating:(BOOL)animate;
- (void)startAnimating;
- (void)stopAnimating;
- (BOOL)isAnimating;

@end
