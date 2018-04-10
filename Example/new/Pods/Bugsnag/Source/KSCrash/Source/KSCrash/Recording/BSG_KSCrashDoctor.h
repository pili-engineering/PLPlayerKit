//
//  BSG_KSCrashDoctor.h
//  BSG_KSCrash
//
//  Created by Karl Stenerud on 2012-11-10.
//  Copyright (c) 2012 Karl Stenerud. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BSG_KSCrashDoctor : NSObject

+ (BSG_KSCrashDoctor *)doctor;

- (NSString *)diagnoseCrash:(NSDictionary *)crashReport;

@end
