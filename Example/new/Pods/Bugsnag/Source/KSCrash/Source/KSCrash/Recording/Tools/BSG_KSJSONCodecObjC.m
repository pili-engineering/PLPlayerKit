//
// BSG_KSJSONCodecObjC.m
//
//  Created by Karl Stenerud on 2012-01-08.
//
//  Copyright (c) 2012 Karl Stenerud. All rights reserved.
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

#import "BSG_KSJSONCodecObjC.h"

#import "BSG_KSJSONCodec.h"
#import "BSG_RFC3339DateTool.h"
#import "NSError+BSG_SimpleConstructor.h"

@interface BSG_KSJSONCodec ()

#pragma mark Properties

/** Callbacks from the C library */
@property(nonatomic, readwrite, assign) BSG_KSJSONDecodeCallbacks *callbacks;

/** Stack of arrays/objects as the decoded content is built */
@property(nonatomic, readwrite, retain) NSMutableArray *containerStack;

/** Current array or object being decoded (weak ref) */
@property(nonatomic, readwrite, assign) id currentContainer;

/** Top level array or object in the decoded tree */
@property(nonatomic, readwrite, retain) id topLevelContainer;

/** Data that has been serialized into JSON form */
@property(nonatomic, readwrite, retain) NSMutableData *serializedData;

/** Any error that has occurred */
@property(nonatomic, readwrite, retain) NSError *error;

/** If true, pretty print while encoding */
@property(nonatomic, readwrite, assign) bool prettyPrint;

/** If true, sort object keys while encoding */
@property(nonatomic, readwrite, assign) bool sorted;

/** If true, don't store nulls in arrays */
@property(nonatomic, readwrite, assign) bool ignoreNullsInArrays;

/** If true, don't store nulls in objects */
@property(nonatomic, readwrite, assign) bool ignoreNullsInObjects;

#pragma mark Constructors

/** Convenience constructor.
 *
 * @param encodeOptions Optional behavior when encoding to JSON.
 *
 * @param decodeOptions Optional behavior when decoding from JSON.
 *
 * @return A new codec.
 */
+ (BSG_KSJSONCodec *)
codecWithEncodeOptions:(BSG_KSJSONEncodeOption)encodeOptions
         decodeOptions:(BSG_KSJSONDecodeOption)decodeOptions;

/** Initializer.
 *
 * @param encodeOptions Optional behavior when encoding to JSON.
 *
 * @param decodeOptions Optional behavior when decoding from JSON.
 *
 * @return The initialized codec.
 */
- (id)initWithEncodeOptions:(BSG_KSJSONEncodeOption)encodeOptions
              decodeOptions:(BSG_KSJSONDecodeOption)decodeOptions;

#pragma mark Callbacks

// Avoiding static functions due to linker issues.

/** Called when a new JSON element is decoded.
 *
 * @param codec The JSON codec.
 *
 * @param name The element's name (or nil for no name)
 *
 * @param element The decoded element.
 *
 * @return BSG_KSJSON_OK, or an error code.
 */
int bsg_ksjsoncodecobjc_i_onElement(BSG_KSJSONCodec *codec, NSString *name,
                                    id element);

/** Called when a new container is encountered while decoding
 *
 * @param codec The JSON codec.
 *
 * @param name The element's name (or nil for no name)
 *
 * @param container The container element.
 *
 * @return BSG_KSJSON_OK, or an error code.
 */
int bsg_ksjsoncodecobjc_i_onBeginContainer(BSG_KSJSONCodec *codec,
                                           NSString *name, id container);

int bsg_ksjsoncodecobjc_i_encodeObject(BSG_KSJSONCodec *codec, id object,
                                       NSString *name,
                                       BSG_KSJSONEncodeContext *context);

/* Various callbacks.
 */
int bsg_ksjsoncodecobjc_i_onBooleanElement(const char *const cName,
                                           const bool value,
                                           void *const userData);

int bsg_ksjsoncodecobjc_i_onFloatingPointElement(const char *const cName,
                                                 const double value,
                                                 void *const userData);

int bsg_ksjsoncodecobjc_i_onIntegerElement(const char *const cName,
                                           const long long value,
                                           void *const userData);

int bsg_ksjsoncodecobjc_i_onNullElement(const char *const cName,
                                        void *const userData);

int bsg_ksjsoncodecobjc_i_onStringElement(const char *const cName,
                                          const char *const value,
                                          void *const userData);

int bsg_ksjsoncodecobjc_i_onBeginObject(const char *const cName,
                                        void *const userData);

int bsg_ksjsoncodecobjc_i_onBeginArray(const char *const cName,
                                       void *const userData);

int bsg_ksjsoncodecobjc_i_onEndContainer(void *const userData);

int bsg_ksjsoncodecobjc_i_onEndData(void *const userData);

int bsg_ksjsoncodecobjc_i_addJSONData(const char *const bytes,
                                      const size_t length,
                                      void *const userData);

@end

#pragma mark -
#pragma mark -

@implementation BSG_KSJSONCodec

#pragma mark Properties

@synthesize topLevelContainer = _topLevelContainer;
@synthesize currentContainer = _currentContainer;
@synthesize containerStack = _containerStack;
@synthesize callbacks = _callbacks;
@synthesize serializedData = _serializedData;
@synthesize error = _error;
@synthesize prettyPrint = _prettyPrint;
@synthesize sorted = _sorted;
@synthesize ignoreNullsInArrays = _ignoreNullsInArrays;
@synthesize ignoreNullsInObjects = _ignoreNullsInObjects;

#pragma mark Constructors/Destructor

+ (BSG_KSJSONCodec *)
codecWithEncodeOptions:(BSG_KSJSONEncodeOption)encodeOptions
         decodeOptions:(BSG_KSJSONDecodeOption)decodeOptions {
    return [[self alloc] initWithEncodeOptions:encodeOptions
                                 decodeOptions:decodeOptions];
}

- (id)initWithEncodeOptions:(BSG_KSJSONEncodeOption)encodeOptions
              decodeOptions:(BSG_KSJSONDecodeOption)decodeOptions {
    if ((self = [super init])) {
        self.containerStack = [NSMutableArray array];
        self.callbacks = malloc(sizeof(*self.callbacks));
        self.callbacks->onBeginArray = bsg_ksjsoncodecobjc_i_onBeginArray;
        self.callbacks->onBeginObject = bsg_ksjsoncodecobjc_i_onBeginObject;
        self.callbacks->onBooleanElement =
            bsg_ksjsoncodecobjc_i_onBooleanElement;
        self.callbacks->onEndContainer = bsg_ksjsoncodecobjc_i_onEndContainer;
        self.callbacks->onEndData = bsg_ksjsoncodecobjc_i_onEndData;
        self.callbacks->onFloatingPointElement =
            bsg_ksjsoncodecobjc_i_onFloatingPointElement;
        self.callbacks->onIntegerElement =
            bsg_ksjsoncodecobjc_i_onIntegerElement;
        self.callbacks->onNullElement = bsg_ksjsoncodecobjc_i_onNullElement;
        self.callbacks->onStringElement = bsg_ksjsoncodecobjc_i_onStringElement;
        self.prettyPrint = (encodeOptions & BSG_KSJSONEncodeOptionPretty) != 0;
        self.sorted = (encodeOptions & BSG_KSJSONEncodeOptionSorted) != 0;
        self.ignoreNullsInArrays =
            (decodeOptions & BSG_KSJSONDecodeOptionIgnoreNullInArray) != 0;
        self.ignoreNullsInObjects =
            (decodeOptions & BSG_KSJSONDecodeOptionIgnoreNullInObject) != 0;
    }
    return self;
}

- (void)dealloc {
    free(self.callbacks);
}

#pragma mark Utility

static inline NSString *stringFromCString(const char *const string) {
    if (string == NULL) {
        return nil;
    }
    return [NSString stringWithCString:string encoding:NSUTF8StringEncoding];
}

#pragma mark Callbacks

int bsg_ksjsoncodecobjc_i_onElement(BSG_KSJSONCodec *codec, NSString *name,
                                    id element) {
    if (codec->_currentContainer == nil) {
        codec.error = [NSError
            bsg_errorWithDomain:@"KSJSONCodecObjC"
                           code:0
                    description:@"Type %@ not allowed as top level container",
                                [element class]];
        return BSG_KSJSON_ERROR_INVALID_DATA;
    }

    if ([codec->_currentContainer isKindOfClass:[NSMutableDictionary class]]) {
        [(NSMutableDictionary *)codec->_currentContainer setValue:element
                                                           forKey:name];
    } else {
        [(NSMutableArray *)codec->_currentContainer addObject:element];
    }
    return BSG_KSJSON_OK;
}

int bsg_ksjsoncodecobjc_i_onBeginContainer(BSG_KSJSONCodec *codec,
                                           NSString *name, id container) {
    if (codec->_topLevelContainer == nil) {
        codec->_topLevelContainer = container;
    } else {
        int result = bsg_ksjsoncodecobjc_i_onElement(codec, name, container);
        if (result != BSG_KSJSON_OK) {
            return result;
        }
    }
    codec->_currentContainer = container;
    [codec->_containerStack addObject:container];
    return BSG_KSJSON_OK;
}

int bsg_ksjsoncodecobjc_i_onBooleanElement(const char *const cName,
                                           const bool value,
                                           void *const userData) {
    NSString *name = stringFromCString(cName);
    id element = @(value);
    BSG_KSJSONCodec *codec = (__bridge BSG_KSJSONCodec *)userData;
    return bsg_ksjsoncodecobjc_i_onElement(codec, name, element);
}

int bsg_ksjsoncodecobjc_i_onFloatingPointElement(const char *const cName,
                                                 const double value,
                                                 void *const userData) {
    NSString *name = stringFromCString(cName);
    id element = @(value);
    BSG_KSJSONCodec *codec = (__bridge BSG_KSJSONCodec *)userData;
    return bsg_ksjsoncodecobjc_i_onElement(codec, name, element);
}

int bsg_ksjsoncodecobjc_i_onIntegerElement(const char *const cName,
                                           const long long value,
                                           void *const userData) {
    NSString *name = stringFromCString(cName);
    id element = @(value);
    BSG_KSJSONCodec *codec = (__bridge BSG_KSJSONCodec *)userData;
    return bsg_ksjsoncodecobjc_i_onElement(codec, name, element);
}

int bsg_ksjsoncodecobjc_i_onNullElement(const char *const cName,
                                        void *const userData) {
    NSString *name = stringFromCString(cName);
    BSG_KSJSONCodec *codec = (__bridge BSG_KSJSONCodec *)userData;

    if ((codec->_ignoreNullsInArrays &&
         [codec->_currentContainer isKindOfClass:[NSArray class]]) ||
        (codec->_ignoreNullsInObjects &&
         [codec->_currentContainer isKindOfClass:[NSDictionary class]])) {
        return BSG_KSJSON_OK;
    }

    return bsg_ksjsoncodecobjc_i_onElement(codec, name, [NSNull null]);
}

int bsg_ksjsoncodecobjc_i_onStringElement(const char *const cName,
                                          const char *const value,
                                          void *const userData) {
    NSString *name = stringFromCString(cName);
    id element =
        [NSString stringWithCString:value encoding:NSUTF8StringEncoding];
    BSG_KSJSONCodec *codec = (__bridge BSG_KSJSONCodec *)userData;
    return bsg_ksjsoncodecobjc_i_onElement(codec, name, element);
}

int bsg_ksjsoncodecobjc_i_onBeginObject(const char *const cName,
                                        void *const userData) {
    NSString *name = stringFromCString(cName);
    id container = [NSMutableDictionary dictionary];
    BSG_KSJSONCodec *codec = (__bridge BSG_KSJSONCodec *)userData;
    return bsg_ksjsoncodecobjc_i_onBeginContainer(codec, name, container);
}

int bsg_ksjsoncodecobjc_i_onBeginArray(const char *const cName,
                                       void *const userData) {
    NSString *name = stringFromCString(cName);
    id container = [NSMutableArray array];
    BSG_KSJSONCodec *codec = (__bridge BSG_KSJSONCodec *)userData;
    return bsg_ksjsoncodecobjc_i_onBeginContainer(codec, name, container);
}

int bsg_ksjsoncodecobjc_i_onEndContainer(void *const userData) {
    BSG_KSJSONCodec *codec = (__bridge BSG_KSJSONCodec *)userData;

    if ([codec->_containerStack count] == 0) {
        codec.error = [NSError
            bsg_errorWithDomain:@"KSJSONCodecObjC"
                           code:0
                    description:
                           @"Already at the top level; no container left to end"];
        return BSG_KSJSON_ERROR_INVALID_DATA;
    }
    [codec->_containerStack removeLastObject];
    NSUInteger count = [codec->_containerStack count];
    if (count > 0) {
        codec->_currentContainer =
                codec->_containerStack[count - 1];
    } else {
        codec->_currentContainer = nil;
    }
    return BSG_KSJSON_OK;
}

int bsg_ksjsoncodecobjc_i_onEndData(__unused void *const userData) {
    return BSG_KSJSON_OK;
}

int bsg_ksjsoncodecobjc_i_addJSONData(const char *const bytes,
                                      const size_t length,
                                      void *const userData) {
    NSMutableData *data = (__bridge NSMutableData *)userData;
    [data appendBytes:bytes length:length];
    return BSG_KSJSON_OK;
}

int bsg_ksjsoncodecobjc_i_encodeObject(BSG_KSJSONCodec *codec, id object,
                                       NSString *name,
                                       BSG_KSJSONEncodeContext *context) {
    int result;
    const char *cName = [name UTF8String];
    if ([object isKindOfClass:[NSString class]]) {
        NSData *data = [object dataUsingEncoding:NSUTF8StringEncoding];
        result = bsg_ksjsonaddStringElement(context, cName, [data bytes],
                                            [data length]);
        if (result == BSG_KSJSON_ERROR_INVALID_CHARACTER) {
            codec.error =
                [NSError bsg_errorWithDomain:@"KSJSONCodecObjC"
                                        code:0
                                 description:@"Invalid character in %@", object];
        }
        return result;
    }

    if ([object isKindOfClass:[NSNumber class]]) {
        switch (CFNumberGetType((__bridge CFNumberRef)object)) {
        case kCFNumberFloat32Type:
        case kCFNumberFloat64Type:
        case kCFNumberFloatType:
        case kCFNumberCGFloatType:
        case kCFNumberDoubleType:
            return bsg_ksjsonaddFloatingPointElement(context, cName,
                                                     [object doubleValue]);
        case kCFNumberCharType:
            return bsg_ksjsonaddBooleanElement(context, cName,
                                               [object boolValue]);
        default:
            return bsg_ksjsonaddIntegerElement(context, cName,
                                               [object longLongValue]);
        }
    }

    if ([object isKindOfClass:[NSArray class]]) {
        if ((result = bsg_ksjsonbeginArray(context, cName)) != BSG_KSJSON_OK) {
            return result;
        }
        if (codec->_sorted) {
            object = [object sortedArrayUsingComparator:^NSComparisonResult(
                                 id obj1, id obj2) {
              Class cls1 = [obj1 class];
              Class cls2 = [obj2 class];
              if (cls1 == cls2) {
                  if ([obj1 respondsToSelector:@selector(compare:)]) {
                      // Cast to keep the compiler happy.
                      return [(NSNumber *)obj1 compare:obj2];
                  }
                  return NSOrderedSame;
              }
              if (cls1 == [NSString class] && cls2 == [NSNumber class]) {
                  return [(NSString *)obj1
                      compare:[NSString stringWithFormat:@"%@", obj2]];
              }
              if (cls1 == [NSNumber class] && cls2 == [NSString class]) {
                  return [(NSString *)[NSString stringWithFormat:@"%@", obj1]
                      compare:obj2];
              }
              return NSOrderedSame;
            }];
        }
        for (id subObject in object) {
            if ((result = bsg_ksjsoncodecobjc_i_encodeObject(
                     codec, subObject, NULL, context)) != BSG_KSJSON_OK) {
                return result;
            }
        }
        return bsg_ksjsonendContainer(context);
    }

    if ([object isKindOfClass:[NSDictionary class]]) {
        if ((result = bsg_ksjsonbeginObject(context, cName)) != BSG_KSJSON_OK) {
            return result;
        }
        NSArray *keys = [object allKeys];
        if (codec->_sorted) {
            keys = [keys sortedArrayUsingSelector:@selector(compare:)];
        }
        for (id key in keys) {
            if ((result = bsg_ksjsoncodecobjc_i_encodeObject(
                     codec, [object valueForKey:key], key, context)) !=
                BSG_KSJSON_OK) {
                return result;
            }
        }
        return bsg_ksjsonendContainer(context);
    }

    if ([object isKindOfClass:[NSNull class]]) {
        return bsg_ksjsonaddNullElement(context, cName);
    }

    if ([object isKindOfClass:[NSDate class]]) {
        NSData *data = [[BSG_RFC3339DateTool stringFromDate:object]
            dataUsingEncoding:NSUTF8StringEncoding];
        return bsg_ksjsonaddStringElement(context, cName, [data bytes],
                                          [data length]);
    }

    if ([object isKindOfClass:[NSData class]]) {
        NSData *data = (NSData *)object;
        return bsg_ksjsonaddDataElement(context, cName, [data bytes],
                                        [data length]);
    }

    codec.error = [NSError
        bsg_errorWithDomain:@"KSJSONCodecObjC"
                       code:0
                description:@"Could not determine type of %@", [object class]];
    return BSG_KSJSON_ERROR_INVALID_DATA;
}

#pragma mark Public API

+ (NSData *)encode:(id)object
           options:(BSG_KSJSONEncodeOption)encodeOptions
             error:(NSError *__autoreleasing *)error {
    NSMutableData *data = [NSMutableData data];
    BSG_KSJSONEncodeContext JSONContext;
    bsg_ksjsonbeginEncode(
        &JSONContext, encodeOptions & BSG_KSJSONEncodeOptionPretty,
        bsg_ksjsoncodecobjc_i_addJSONData, (__bridge void *)data);
    BSG_KSJSONCodec *codec =
        [self codecWithEncodeOptions:encodeOptions decodeOptions:0];

    int result =
        bsg_ksjsoncodecobjc_i_encodeObject(codec, object, NULL, &JSONContext);
    if (error != nil) {
        *error = codec.error;
    }
    return result == BSG_KSJSON_OK ? data : nil;
}

+ (id)decode:(NSData *)JSONData
     options:(BSG_KSJSONDecodeOption)decodeOptions
       error:(NSError *__autoreleasing *)error {
    BSG_KSJSONCodec *codec =
        [self codecWithEncodeOptions:0 decodeOptions:decodeOptions];
    size_t errorOffset;
    int result =
        bsg_ksjsondecode([JSONData bytes], [JSONData length], codec.callbacks,
                         (__bridge void *)codec, &errorOffset);
    if (result != BSG_KSJSON_OK && codec.error == nil) {
        codec.error = [NSError
           bsg_errorWithDomain:@"KSJSONCodecObjC"
                          code:0
                   description:@"%s (offset %d)", bsg_ksjsonstringForError(result),
                               errorOffset];
    }
    if (error != nil) {
        *error = codec.error;
    }

    if (result != BSG_KSJSON_OK &&
        !(decodeOptions & BSG_KSJSONDecodeOptionKeepPartialObject)) {
        return nil;
    }
    return codec.topLevelContainer;
}

@end
