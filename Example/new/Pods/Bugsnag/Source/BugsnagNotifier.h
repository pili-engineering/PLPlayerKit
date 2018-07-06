//
//  BugsnagMetaData.m
//
//  Created by Conrad Irwin on 2014-10-01.
//
//  Copyright (c) 2014 Bugsnag, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall remain in place
// in this source code.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#import <Foundation/Foundation.h>

#import "BugsnagConfiguration.h"
#import "BugsnagMetaData.h"

@class BSGConnectivity;

@interface BugsnagNotifier : NSObject <BugsnagMetaDataDelegate>

@property(nonatomic, readwrite, retain)
    BugsnagConfiguration *_Nullable configuration;
@property(nonatomic, readwrite, retain) BugsnagMetaData *_Nonnull state;
@property(nonatomic, readwrite, retain) NSDictionary *_Nonnull details;
@property(nonatomic, readwrite, retain) NSLock *_Nonnull metaDataLock;

@property(nonatomic) BSGConnectivity *_Nonnull networkReachable;
@property(readonly) BOOL started;

- (instancetype _Nonnull)initWithConfiguration:
    (BugsnagConfiguration *_Nonnull)configuration;
- (void)start;

- (void)startSession;

/**
 *  Notify Bugsnag of an exception
 *
 *  @param exception the exception
 *  @param block     Configuration block for adding additional report
 * information
 */
- (void)notifyException:(NSException *_Nonnull)exception
                  block:(BugsnagNotifyBlock _Nullable)block;

/**
 *  Notify Bugsnag of an exception
 *
 *  @param exception the exception
 *  @param severity  the severity
 *  @param block     Configuration block for adding additional report
 * information
 */
- (void)notifyException:(NSException *_Nonnull)exception
             atSeverity:(BSGSeverity)severity
                  block:(BugsnagNotifyBlock _Nullable)block;

/**
 *  Notify Bugsnag of an exception. Only intended for React Native/Unity use.
 *
 *  @param exception the exception
 *  @param metaData  the metadata
 *  @param block     Configuration block for adding additional report
 * information
 */
- (void)internalClientNotify:(NSException *_Nonnull)exception
                    withData:(NSDictionary *_Nullable)metaData
                       block:(BugsnagNotifyBlock _Nullable)block;

/**
 *  Notify Bugsnag of an error
 *
 *  @param error the error
 *  @param block Configuration block for adding additional report information
 */
- (void)notifyError:(NSError *_Nonnull)error
              block:(BugsnagNotifyBlock _Nullable)block;

/**
 *  Add a breadcrumb
 *
 *  @param block configuration block
 */
- (void)addBreadcrumbWithBlock:
    (void (^_Nonnull)(BugsnagBreadcrumb *_Nonnull))block;

/**
 * Clear all stored breadcrumbs.
 */
- (void)clearBreadcrumbs;

/**
 *  Listen for notifications and attach breadcrumbs when received
 *
 *  @param notificationName name of the notification
 */
- (void)crumbleNotification:(NSString *_Nonnull)notificationName;

/**
 *  Enable or disable automatic breadcrumb collection based on configuration
 */
- (void)updateAutomaticBreadcrumbDetectionSettings;

@end
