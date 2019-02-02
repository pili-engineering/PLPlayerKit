//
//  BugsnagCrashSentry.h
//  Pods
//
//  Created by Jamie Lynch on 11/08/2017.
//
//

#import <Foundation/Foundation.h>

#import "BSG_KSCrashReportWriter.h"
#import "BugsnagConfiguration.h"
#import "BugsnagErrorReportApiClient.h"

@interface BugsnagCrashSentry : NSObject

- (void)install:(BugsnagConfiguration *)config
      apiClient:(BugsnagErrorReportApiClient *)apiClient
        onCrash:(BSGReportCallback)onCrash;

- (void)reportUserException:(NSString *)reportName
                     reason:(NSString *)reportMessage
               handledState:(NSDictionary *)handledState
                   appState:(NSDictionary *)appState
          callbackOverrides:(NSDictionary *)overrides
                   metadata:(NSDictionary *)metadata
                     config:(NSDictionary *)config
               discardDepth:(int)depth;

@end
