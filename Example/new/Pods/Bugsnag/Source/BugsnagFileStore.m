//
// Created by Jamie Lynch on 29/11/2017.
// Copyright (c) 2017 Bugsnag. All rights reserved.
//

#import "BugsnagFileStore.h"
#import "BSG_KSCrashReportFields.h"
#import "BSG_KSJSONCodecObjC.h"
#import "NSError+BSG_SimpleConstructor.h"
#import "BSG_KSLogger.h"

#pragma mark - Meta Data


/**
 * Metadata class to hold name and creation date for a file, with
 * default comparison based on the creation date (ascending).
 */
@interface FileStoreInfo : NSObject

@property(nonatomic, readonly, retain) NSString *fileId;
@property(nonatomic, readonly, retain) NSDate *creationDate;

+ (FileStoreInfo *)fileStoreInfoWithId:(NSString *)fileId
                          creationDate:(NSDate *)creationDate;

- (instancetype)initWithId:(NSString *)fileId creationDate:(NSDate *)creationDate;

- (NSComparisonResult)compare:(FileStoreInfo *)other;

@end

@implementation FileStoreInfo

@synthesize fileId = _fileId;
@synthesize creationDate = _creationDate;

+ (FileStoreInfo *)fileStoreInfoWithId:(NSString *)fileId
                          creationDate:(NSDate *)creationDate {
    return [[self alloc] initWithId:fileId creationDate:creationDate];
}

- (instancetype)initWithId:(NSString *)fileId creationDate:(NSDate *)creationDate {
    if ((self = [super init])) {
        _fileId = fileId;
        _creationDate = creationDate;
    }
    return self;
}

- (NSComparisonResult)compare:(FileStoreInfo *)other {
    return [_creationDate compare:other->_creationDate];
}

@end

#pragma mark - Main Class


@interface BugsnagFileStore ()

@property(nonatomic, readwrite, retain) NSString *path;

@end


@implementation BugsnagFileStore

#pragma mark Properties

@synthesize path = _path;

#pragma mark Construction

- (instancetype)initWithPath:(NSString *)path
              filenameSuffix:(NSString *)filenameSuffix {
    if ((self = [super init])) {
        self.path = path;
        _filenameSuffix = filenameSuffix;
        self.bundleName = [NSBundle.mainBundle infoDictionary][@"CFBundleName"];
    }
    return self;
}

#pragma mark API

- (NSArray *)fileIds {
    NSError *error = nil;
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray *filenames = [fm contentsOfDirectoryAtPath:self.path error:&error];
    if (filenames == nil) {
        BSG_KSLOG_ERROR(@"Could not get contents of directory %@: %@",
                self.path, error);
        return nil;
    }

    NSMutableArray *files = [NSMutableArray arrayWithCapacity:[filenames count]];

    for (NSString *filename in filenames) {
        NSString *fileId = [self fileIdFromFilename:filename];
        if (fileId != nil) {
            NSString *fullPath =
                    [self.path stringByAppendingPathComponent:filename];
            NSDictionary *fileAttribs =
                    [fm attributesOfItemAtPath:fullPath error:&error];
            if (fileAttribs == nil) {
                BSG_KSLOG_ERROR(@"Could not read file attributes for %@: %@",
                        fullPath, error);
            } else {
                FileStoreInfo *info = [FileStoreInfo fileStoreInfoWithId:fileId
                                                            creationDate:[fileAttribs valueForKey:NSFileCreationDate]];
                [files addObject:info];
            }
        }
    }
    [files sortUsingSelector:@selector(compare:)];

    NSMutableArray *sortedIDs =
            [NSMutableArray arrayWithCapacity:[files count]];
    for (FileStoreInfo *info in files) {
        [sortedIDs addObject:info.fileId];
    }
    return sortedIDs;
}

- (NSUInteger)fileCount {
    return [self.fileIds count];
}

- (NSArray *)allFiles {
    NSArray *fileIds = [self fileIds];
    NSMutableArray *files =
            [NSMutableArray arrayWithCapacity:[fileIds count]];
    for (NSString *fileId in fileIds) {
        NSDictionary *fileContents = [self fileWithId:fileId];
        if (fileContents != nil) {
            [files addObject:fileContents];
        }
    }

    return files;
}

- (void)deleteAllFiles {
    for (NSString *fileId in [self fileIds]) {
        [self deleteFileWithId:fileId];
    }
}

- (void)pruneFilesLeaving:(int)numFiles {
    NSArray *fileIds = [self fileIds];
    int deleteCount = (int) [fileIds count] - numFiles;
    for (int i = 0; i < deleteCount; i++) {
        [self deleteFileWithId:fileIds[(NSUInteger) i]];
    }
}

- (NSDictionary *)fileWithId:(NSString *)fileId {
    NSError *error = nil;
    NSMutableDictionary *fileContents =
            [self readFile:[self pathToFileWithId:fileId] error:&error];
    if (error != nil) {
        BSG_KSLOG_ERROR(@"Encountered error loading file %@: %@",
                fileId, error);
    }
    if (fileContents == nil) {
        BSG_KSLOG_ERROR(@"Could not load file");
        return nil;
    }
    return fileContents;
}

- (void)deleteFileWithId:(NSString *)fileId {
    NSError *error = nil;
    NSString *filename = [self pathToFileWithId:fileId];

    [[NSFileManager defaultManager] removeItemAtPath:filename error:&error];
    if (error != nil) {
        BSG_KSLOG_ERROR(@"Could not delete file %@: %@", filename, error);
    }
}

+ (NSString *)findReportStorePath:(NSString *)customDirectory  {

    NSString *bundleName = [[NSBundle mainBundle] infoDictionary][@"CFBundleName"];
    NSArray *directories = NSSearchPathForDirectoriesInDomains(
            NSCachesDirectory, NSUserDomainMask, YES);
    if ([directories count] == 0) {
        BSG_KSLOG_ERROR(@"Could not locate cache directory path.");
        return nil;
    }

    NSString *cachePath = directories[0];

    if ([cachePath length] == 0) {
        BSG_KSLOG_ERROR(@"Could not locate cache directory path.");
        return nil;
    }

    NSString *storePathEnd = [customDirectory
            stringByAppendingPathComponent:bundleName];

    NSString *storePath =
            [cachePath stringByAppendingPathComponent:storePathEnd];

    if ([storePath length] == 0) {
        BSG_KSLOG_ERROR(@"Could not determine report files path.");
        return nil;
    }
    if (![self ensureDirectoryExists:storePath]) {
        BSG_KSLOG_ERROR(@"Store Directory does not exist.");
        return nil;
    }
    return storePath;
}

+ (BOOL)ensureDirectoryExists:(NSString *)path {
    NSError *error = nil;
    NSFileManager *fm = [NSFileManager defaultManager];

    if (![fm fileExistsAtPath:path]) {
        if (![fm createDirectoryAtPath:path
           withIntermediateDirectories:YES
                            attributes:nil
                                 error:&error]) {
            BSG_KSLOG_ERROR(@"Could not create directory %@: %@.", path, error);
            return NO;
        }
    }

    return YES;
}

#pragma mark Utility

- (void)performOnFields:(NSArray *)fieldPath
                 inFile:(NSMutableDictionary *)file
              operation:(void (^)(id parent, id field))operation
           okIfNotFound:(BOOL)isOkIfNotFound {
    if (fieldPath.count == 0) {
        BSG_KSLOG_ERROR(@"Unexpected end of field path");
        return;
    }

    NSString *currentField = fieldPath[0];
    if (fieldPath.count > 1) {
        fieldPath =
                [fieldPath subarrayWithRange:NSMakeRange(1, fieldPath.count - 1)];
    } else {
        fieldPath = @[];
    }

    id field = file[currentField];
    if (field == nil) {
        if (!isOkIfNotFound) {
            BSG_KSLOG_ERROR(@"%@: No such field in file. Candidates are: %@",
                    currentField, file.allKeys);
        }
        return;
    }

    if ([field isKindOfClass:NSMutableDictionary.class]) {
        [self performOnFields:fieldPath
                       inFile:field
                    operation:operation
                 okIfNotFound:isOkIfNotFound];
    } else if ([field isKindOfClass:[NSMutableArray class]]) {
        for (id subfield in field) {
            if ([subfield isKindOfClass:NSMutableDictionary.class]) {
                [self performOnFields:fieldPath
                               inFile:subfield
                            operation:operation
                         okIfNotFound:isOkIfNotFound];
            } else {
                operation(field, subfield);
            }
        }
    } else {
        operation(file, field);
    }
}

- (NSString *)pathToFileWithId:(NSString *)fileId {
    NSString *filename = [self filenameWithId:fileId];
    return [self.path stringByAppendingPathComponent:filename];
}

- (NSMutableDictionary *)readFile:(NSString *)path
                            error:(NSError *__autoreleasing *)error {
    if (path == nil) {
        [NSError bsg_fillError:error
                    withDomain:[[self class] description]
                          code:0
                   description:@"Path is nil"];
        return nil;
    }

    NSData *jsonData =
            [NSData dataWithContentsOfFile:path options:0 error:error];
    if (jsonData == nil) {
        return nil;
    }

    NSMutableDictionary *fileContents =
            [BSG_KSJSONCodec decode:jsonData
                            options:BSG_KSJSONDecodeOptionIgnoreNullInArray |
                                    BSG_KSJSONDecodeOptionIgnoreNullInObject |
                                    BSG_KSJSONDecodeOptionKeepPartialObject
                              error:error];
    if (error != nil && *error != nil) {

        BSG_KSLOG_ERROR(@"Error decoding JSON data from %@: %@", path, *error);
        fileContents[@BSG_KSCrashField_Incomplete] = @YES;
    }
    return fileContents;
}


- (NSString *)filenameWithId:(NSString *)fileId {
    // e.g. Bugsnag Test App-CrashReport-54D4FF86-C3D1-4167-8485-3D7539FDFFF5.json
    return [NSString stringWithFormat:@"%@%@%@.json", self.bundleName, self.filenameSuffix, fileId];
}

- (NSString *)fileIdFromFilename:(NSString *)filename {
    if ([filename length] == 0 ||
            ![[filename pathExtension] isEqualToString:@"json"]) {
        return nil;
    }

    NSString *prefix = [NSString stringWithFormat:@"%@%@", self.bundleName, self.filenameSuffix];
    NSString *suffix = @".json";

    NSRange prefixRange = [filename rangeOfString:prefix];
    NSRange suffixRange =
            [filename rangeOfString:suffix options:NSBackwardsSearch];
    if (prefixRange.location == 0 && suffixRange.location != NSNotFound) {
        NSUInteger prefixEnd = NSMaxRange(prefixRange);
        NSRange range =
                NSMakeRange(prefixEnd, suffixRange.location - prefixEnd);
        return [filename substringWithRange:range];
    }
    return nil;
}


@end
