#ifndef BugsnagJSONSerializable_h
#define BugsnagJSONSerializable_h

#import <Foundation/Foundation.h>

/**
 Removes any values which would be rejected by NSJSONSerialization for
 documented reasons

 @param input an array
 @return a new array
 */
NSArray *_Nonnull BSGSanitizeArray(NSArray *_Nonnull input);

/**
 Removes any values which would be rejected by NSJSONSerialization for
 documented reasons

 @param input a dictionary
 @return a new dictionary
 */
NSDictionary *_Nonnull BSGSanitizeDict(NSDictionary *_Nonnull input);

/**
 Checks whether the base type would be accepted by the serialization process

 @param obj any object or nil
 @return YES if the object is an Array, Dictionary, String, Number, or NSNull
 */
BOOL BSGIsSanitizedType(id _Nullable obj);

/**
 Cleans the object, including nested dictionary and array values

 @param obj any object or nil
 @return a new object for serialization or nil if the obj was incompatible
 */
id _Nullable BSGSanitizeObject(id _Nullable obj);

#endif
