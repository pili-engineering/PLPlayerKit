//
//  BugsnagUser.m
//  Bugsnag
//
//  Created by Jamie Lynch on 24/11/2017.
//  Copyright © 2017 Bugsnag. All rights reserved.
//

#import "BugsnagUser.h"
#import "BugsnagCollections.h"

@implementation BugsnagUser

- (instancetype)initWithDictionary:(NSDictionary *)dict {
    if (self = [super init]) {
        _userId = dict[@"id"];
        _emailAddress = dict[@"email"];
        _name = dict[@"name"];
    }
    return self;
}

- (instancetype)initWithUserId:(NSString *)userId name:(NSString *)name emailAddress:(NSString *)emailAddress {
    self = [super init];
    if (self) {
        self.userId = userId;
        self.name = name;
        self.emailAddress = emailAddress;
    }
    return self;
}

+ (instancetype)userWithUserId:(NSString *)userId name:(NSString *)name emailAddress:(NSString *)emailAddress {
    return [[self alloc] initWithUserId:userId name:name emailAddress:emailAddress];
}

- (NSDictionary *)toJson {
    NSMutableDictionary *dict = [NSMutableDictionary new];
    BSGDictInsertIfNotNil(dict, self.userId, @"id");
    BSGDictInsertIfNotNil(dict, self.emailAddress, @"email");
    BSGDictInsertIfNotNil(dict, self.name, @"name");
    return [NSDictionary dictionaryWithDictionary:dict];
}

@end
