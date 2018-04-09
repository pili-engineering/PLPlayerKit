//
// Created by Jamie Lynch on 30/11/2017.
// Copyright (c) 2017 Bugsnag. All rights reserved.
//

#import "BugsnagSessionFileStore.h"
#import "BSG_KSLogger.h"

static NSString *const kSessionStoreSuffix = @"-Session-";

@implementation BugsnagSessionFileStore

+ (BugsnagSessionFileStore *)storeWithPath:(NSString *)path {
    return [[self alloc] initWithPath:path
                       filenameSuffix:kSessionStoreSuffix];
}

- (void)write:(BugsnagSession *)session {
    // serialise session
    NSString *filepath = [self pathToFileWithId:session.sessionId];
    NSDictionary *dict = [session toJson];

    NSError *error;
    NSData *json = [NSJSONSerialization dataWithJSONObject:dict options:0 error:&error];

    if (error != nil || ![json writeToFile:filepath atomically:YES]) {
        BSG_KSLOG_ERROR(@"Failed to write session %@", error);
        return;
    }
}


@end
