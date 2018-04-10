//
//  BSG_KSObjC.c
//
//  Created by Karl Stenerud on 2012-08-30.
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

#include "BSG_KSObjC.h"
#include "BSG_KSObjCApple.h"

#include "BSG_KSMach.h"
#include "BSG_KSString.h"

#if __IPHONE_OS_VERSION_MAX_ALLOWED > 70000
#include <objc/NSObjCRuntime.h>
#else
#if __LP64__ || (TARGET_OS_EMBEDDED && !TARGET_OS_IPHONE) ||                   \
    TARGET_OS_WIN32 || NS_BUILD_32_LIKE_64
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif
#endif

#include <CoreGraphics/CGBase.h>
#include <objc/runtime.h>

#define kMaxNameLength 128

//======================================================================
#pragma mark - Macros -
//======================================================================

// Compiler hints for "if" statements
#define likely_if(x) if (__builtin_expect(x, 1))
#define unlikely_if(x) if (__builtin_expect(x, 0))

//======================================================================
#pragma mark - Types -
//======================================================================

typedef enum {
    ClassSubtypeNone = 0,
    ClassSubtypeCFArray,
    ClassSubtypeNSArrayMutable,
    ClassSubtypeNSArrayImmutable,
    ClassSubtypeCFString,
} ClassSubtype;

typedef struct {
    const char *name;
    BSG_KSObjCClassType type;
    ClassSubtype subtype;
    bool isMutable;
    bool (*isValidObject)(const void *object);
    size_t (*description)(const void *object, char *buffer,
                          size_t bufferLength);
    const void *class;
} ClassData;

//======================================================================
#pragma mark - Globals -
//======================================================================

// Forward references
static bool objectIsValid(const void *object);
static bool taggedObjectIsValid(const void *object);
static bool stringIsValid(const void *object);
static bool urlIsValid(const void *object);
static bool arrayIsValid(const void *object);
static bool dateIsValid(const void *object);
static bool numberIsValid(const void *object);
static bool taggedDateIsValid(const void *object);
static bool taggedNumberIsValid(const void *object);
static bool taggedStringIsValid(const void *object);

static size_t objectDescription(const void *object, char *buffer,
                                size_t bufferLength);
static size_t taggedObjectDescription(const void *object, char *buffer,
                                      size_t bufferLength);
static size_t stringDescription(const void *object, char *buffer,
                                size_t bufferLength);
static size_t urlDescription(const void *object, char *buffer,
                             size_t bufferLength);
static size_t arrayDescription(const void *object, char *buffer,
                               size_t bufferLength);
static size_t dateDescription(const void *object, char *buffer,
                              size_t bufferLength);
static size_t numberDescription(const void *object, char *buffer,
                                size_t bufferLength);
static size_t taggedDateDescription(const void *object, char *buffer,
                                    size_t bufferLength);
static size_t taggedNumberDescription(const void *object, char *buffer,
                                      size_t bufferLength);
static size_t taggedStringDescription(const void *object, char *buffer,
                                      size_t bufferLength);

static ClassData bsg_g_classData[] = {
    {"__NSCFString", BSG_KSObjCClassTypeString, ClassSubtypeNone, true,
     stringIsValid, stringDescription},
    {"NSCFString", BSG_KSObjCClassTypeString, ClassSubtypeNone, true,
     stringIsValid, stringDescription},
    {"__NSCFConstantString", BSG_KSObjCClassTypeString, ClassSubtypeNone, true,
     stringIsValid, stringDescription},
    {"NSCFConstantString", BSG_KSObjCClassTypeString, ClassSubtypeNone, true,
     stringIsValid, stringDescription},
    {"__NSArray0", BSG_KSObjCClassTypeArray, ClassSubtypeNSArrayImmutable,
     false, arrayIsValid, arrayDescription},
    {"__NSArrayI", BSG_KSObjCClassTypeArray, ClassSubtypeNSArrayImmutable,
     false, arrayIsValid, arrayDescription},
    {"__NSArrayM", BSG_KSObjCClassTypeArray, ClassSubtypeNSArrayMutable, true,
     arrayIsValid, arrayDescription},
    {"__NSCFArray", BSG_KSObjCClassTypeArray, ClassSubtypeCFArray, false,
     arrayIsValid, arrayDescription},
    {"NSCFArray", BSG_KSObjCClassTypeArray, ClassSubtypeCFArray, false,
     arrayIsValid, arrayDescription},
    {"__NSDate", BSG_KSObjCClassTypeDate, ClassSubtypeNone, false, dateIsValid,
     dateDescription},
    {"NSDate", BSG_KSObjCClassTypeDate, ClassSubtypeNone, false, dateIsValid,
     dateDescription},
    {"__NSCFNumber", BSG_KSObjCClassTypeNumber, ClassSubtypeNone, false,
     numberIsValid, numberDescription},
    {"NSCFNumber", BSG_KSObjCClassTypeNumber, ClassSubtypeNone, false,
     numberIsValid, numberDescription},
    {"NSNumber", BSG_KSObjCClassTypeNumber, ClassSubtypeNone, false,
     numberIsValid, numberDescription},
    {"NSURL", BSG_KSObjCClassTypeURL, ClassSubtypeNone, false, urlIsValid,
     urlDescription},
    {NULL, BSG_KSObjCClassTypeUnknown, ClassSubtypeNone, false, objectIsValid,
     objectDescription},
};

static ClassData bsg_g_taggedClassData[] = {
    {"NSAtom", BSG_KSObjCClassTypeUnknown, ClassSubtypeNone, false,
     taggedObjectIsValid, taggedObjectDescription},
    {NULL, BSG_KSObjCClassTypeUnknown, ClassSubtypeNone, false,
     taggedObjectIsValid, taggedObjectDescription},
    {"NSString", BSG_KSObjCClassTypeString, ClassSubtypeNone, false,
     taggedStringIsValid, taggedStringDescription},
    {"NSNumber", BSG_KSObjCClassTypeNumber, ClassSubtypeNone, false,
     taggedNumberIsValid, taggedNumberDescription},
    {"NSIndexPath", BSG_KSObjCClassTypeUnknown, ClassSubtypeNone, false,
     taggedObjectIsValid, taggedObjectDescription},
    {"NSManagedObjectID", BSG_KSObjCClassTypeUnknown, ClassSubtypeNone, false,
     taggedObjectIsValid, taggedObjectDescription},
    {"NSDate", BSG_KSObjCClassTypeDate, ClassSubtypeNone, false,
     taggedDateIsValid, taggedDateDescription},
    {NULL, BSG_KSObjCClassTypeUnknown, ClassSubtypeNone, false,
     taggedObjectIsValid, taggedObjectDescription},
};
static size_t bsg_g_taggedClassDataCount =
    sizeof(bsg_g_taggedClassData) / sizeof(*bsg_g_taggedClassData);

static const char *bsg_g_blockBaseClassName = "NSBlock";

//======================================================================
#pragma mark - Utility -
//======================================================================

#if SUPPORT_TAGGED_POINTERS
bool bsg_isTaggedPointer(const void *pointer) {
    return (((uintptr_t)pointer) & TAG_MASK) != 0;
}
uintptr_t bsg_getTaggedSlot(const void *pointer) {
    return (((uintptr_t)pointer) >> TAG_SLOT_SHIFT) & TAG_SLOT_MASK;
}
uintptr_t bsg_getTaggedPayload(const void *pointer) {
    return (((uintptr_t)pointer) << TAG_PAYLOAD_LSHIFT) >> TAG_PAYLOAD_RSHIFT;
}
#else
bool bsg_isTaggedPointer(__unused const void *pointer) { return false; }
uintptr_t bsg_getTaggedSlot(__unused const void *pointer) { return 0; }
uintptr_t bsg_getTaggedPayload(const void *pointer) {
    return (uintptr_t)pointer;
}
#endif

/** Get class data for a tagged pointer.
 *
 * @param object The tagged pointer.
 * @return The class data.
 */
static const ClassData *
getClassDataFromTaggedPointer(const void *const object) {
    uintptr_t slot = bsg_getTaggedSlot(object);
    return &bsg_g_taggedClassData[slot];
}

static bool isValidTaggedPointer(const void *object) {
    if (bsg_isTaggedPointer(object)) {
        if (bsg_getTaggedSlot(object) <= bsg_g_taggedClassDataCount) {
            const ClassData *classData = getClassDataFromTaggedPointer(object);
            return classData->type != BSG_KSObjCClassTypeUnknown;
        }
    }
    return false;
}

const void *bsg_decodeIsaPointer(const void *const isaPointer) {
#if ISA_TAG_MASK
    uintptr_t isa = (uintptr_t)isaPointer;
    if (isa & ISA_TAG_MASK) {
        return (const void *)(isa & ISA_MASK);
    }
#endif
    return isaPointer;
}

static inline bool isValidObject(const void *object) {
    if (bsg_isTaggedPointer(object)) {
        return isValidTaggedPointer(object);
    }

    struct class_t data;
    return bsg_ksmachcopyMem(object, &data, sizeof(data)) == KERN_SUCCESS;
}

static inline bool hasValidISAPointer(const void *object) {
    // Note: Assuming that this isn't a tagged pointer!
    const struct class_t *ptr = object;
    const void *isaPtr = bsg_decodeIsaPointer(ptr->isa);
    struct class_t data;
    return bsg_ksmachcopyMem(isaPtr, &data, sizeof(data)) == KERN_SUCCESS;
}

const void *bsg_getIsaPointer(const void *const objectOrClassPtr) {
    // This is wrong. Should not get class data here.
    //    if(ksobjc_bsg_isTaggedPointer(objectOrClassPtr))
    //    {
    //        return getClassDataFromTaggedPointer(objectOrClassPtr)->class;
    //    }

    const struct class_t *ptr = objectOrClassPtr;
    return bsg_decodeIsaPointer(ptr->isa);
}

static inline struct class_rw_t *getClassRW(const struct class_t *const class) {
    uintptr_t ptr = class->data_NEVER_USE & (~WORD_MASK);
    return (struct class_rw_t *)ptr;
}

static inline const struct class_ro_t *
getClassRO(const struct class_t *const class) {
    return getClassRW(class)->ro;
}

static inline const void *getSuperClass(const void *const classPtr) {
    const struct class_t *class = classPtr;
    return class->superclass;
}

static inline bool isMetaClass(const void *const classPtr) {
    return (getClassRO(classPtr)->flags & RO_META) != 0;
}

static inline bool isRootClass(const void *const classPtr) {
    return (getClassRO(classPtr)->flags & RO_ROOT) != 0;
}

static inline const char *getClassName(const void *classPtr) {
    const struct class_ro_t *ro = getClassRO(classPtr);
    return ro->name;
}

/** Check if a tagged pointer is a number.
 *
 * @param object The object to query.
 * @return true if the tagged pointer is an NSNumber.
 */
static bool bsg_isTaggedPointerNSNumber(const void *const object) {
    return bsg_getTaggedSlot(object) == OBJC_TAG_NSNumber;
}

/** Check if a tagged pointer is a string.
 *
 * @param object The object to query.
 * @return true if the tagged pointer is an NSString.
 */
static bool bsg_isTaggedPointerNSString(const void *const object) {
    return bsg_getTaggedSlot(object) == OBJC_TAG_NSString;
}

/** Check if a tagged pointer is a date.
 *
 * @param object The object to query.
 * @return true if the tagged pointer is an NSDate.
 */
static bool bsg_isTaggedPointerNSDate(const void *const object) {
    return bsg_getTaggedSlot(object) == OBJC_TAG_NSDate;
}

/** Extract an integer from a tagged NSNumber.
 *
 * @param object The NSNumber object (must be a tagged pointer).
 * @return The integer value.
 */
static int64_t extractTaggedNSNumber(const void *const object) {
    intptr_t signedPointer = (intptr_t)object;
#if SUPPORT_TAGGED_POINTERS
    intptr_t value =
        (signedPointer << TAG_PAYLOAD_LSHIFT) >> TAG_PAYLOAD_RSHIFT;
#else
    intptr_t value = signedPointer & 0;
#endif

    // The lower 4 bits encode type information so shift them out.
    return (int64_t)(value >> 4);
}

static size_t getTaggedNSStringLength(const void *const object) {
    uintptr_t payload = bsg_getTaggedPayload(object);
    return payload & 0xf;
}

static size_t extractTaggedNSString(const void *const object, char *buffer,
                                    size_t bufferLength) {
    size_t length = getTaggedNSStringLength(object);
    size_t copyLength =
        ((length + 1) > bufferLength) ? (bufferLength - 1) : length;
    uintptr_t payload = bsg_getTaggedPayload(object);
    uintptr_t value = payload >> 4;
    static char *alphabet =
        "eilotrm.apdnsIc ufkMShjTRxgC4013bDNvwyUL2O856P-B79AFKEWV_zGJ/HYX";
    if (length <= 7) {
        for (size_t i = 0; i < copyLength; i++) {
            buffer[i] = (char)(value & 0xff);
            value >>= 8;
        }
    } else if (length <= 9) {
        for (size_t i = 0; i < copyLength; i++) {
            uintptr_t index = (value >> ((length - 1 - i) * 6)) & 0x3f;
            buffer[i] = alphabet[index];
        }
    } else if (length <= 11) {
        for (size_t i = 0; i < copyLength; i++) {
            uintptr_t index = (value >> ((length - 1 - i) * 5)) & 0x1f;
            buffer[i] = alphabet[index];
        }
    } else {
        buffer[0] = 0;
    }
    buffer[length] = 0;

    return length;
}

/** Extract a tagged NSDate's time value as an absolute time.
 *
 * @param object The NSDate object (must be a tagged pointer).
 * @return The date's absolute time.
 */
static CFAbsoluteTime extractTaggedNSDate(const void *const object) {
    uintptr_t payload = bsg_getTaggedPayload(object);
    // Payload is a 60-bit float. Fortunately we can just cast across from
    // an integer pointer after shifting out the upper 4 bits.
    payload <<= 4;
    CFAbsoluteTime value = *((CFAbsoluteTime *)&payload);
    return value;
}

/** Get any special class metadata we have about the specified class.
 * It will return a generic metadata object if the type is not recognized.
 *
 * Note: The Objective-C runtime is free to change a class address,
 * so I can't just blindly store class pointers at application start
 * and then compare against them later. However, comparing strings is
 * slow, so I've reached a compromise. Since I'm omly using this at
 * crash time, I can assume that the Objective-C environment is frozen.
 * As such, I can keep a cache of discovered classes. If, however, this
 * library is used outside of a frozen environment, caching will be
 * unreliable.
 *
 * @param class The class to examine.
 *
 * @return The associated class data.
 */
static ClassData *getClassData(const void *class) {
    const char *className = getClassName(class);
    for (ClassData *data = bsg_g_classData;; data++) {
        unlikely_if(data->name == NULL) { return data; }
        unlikely_if(class == data->class) { return data; }
        unlikely_if(data->class == NULL && strcmp(className, data->name) == 0) {
            data->class = class;
            return data;
        }
    }
}

static inline const ClassData *getClassDataFromObject(const void *object) {
    if (bsg_isTaggedPointer(object)) {
        return getClassDataFromTaggedPointer(object);
    }
    const struct class_t *obj = object;
    return getClassData(bsg_getIsaPointer(obj));
}

static size_t stringPrintf(char *buffer, size_t bufferLength, const char *fmt,
                           ...) {
    unlikely_if(bufferLength == 0) { return 0; }

    va_list args;
    va_start(args, fmt);
    int printLength = vsnprintf(buffer, bufferLength, fmt, args);
    va_end(args);

    unlikely_if(printLength < 0) {
        *buffer = 0;
        return 0;
    }
    unlikely_if((size_t)printLength > bufferLength) { return bufferLength - 1; }
    return (size_t)printLength;
}

//======================================================================
#pragma mark - Validation -
//======================================================================

// Lookup table for validating class/ivar names and objc @encode types.
// An ivar name must start with a letter, and can contain letters & numbers.
// An ivar type can in theory be any combination of numbers, letters, and
// symbols in the ASCII range (0x21-0x7e).
#define INV 0 // Invalid.
#define N_C                                                                    \
    5 // Name character: Valid for anything except the first letter of a name.
#define N_S 7 // Name start character: Valid for anything.
#define T_C 4 // Type character: Valid for types only.

static const unsigned int bsg_g_nameChars[] = {
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, T_C, T_C, T_C, T_C, T_C, T_C, T_C, T_C, T_C, T_C, T_C, T_C,
    T_C, T_C, T_C, N_C, N_C, N_C, N_C, N_C, N_C, N_C, N_C, N_C, N_C, T_C, T_C,
    T_C, T_C, T_C, T_C, T_C, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S,
    N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S,
    N_S, T_C, T_C, T_C, T_C, N_S, T_C, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S,
    N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S, N_S,
    N_S, N_S, N_S, T_C, T_C, T_C, T_C, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV,
};

#define VALID_NAME_CHAR(A) ((bsg_g_nameChars[(uint8_t)(A)] & 1) != 0)
#define VALID_NAME_START_CHAR(A) ((bsg_g_nameChars[(uint8_t)(A)] & 2) != 0)
#define VALID_TYPE_CHAR(A) ((bsg_g_nameChars[(uint8_t)(A)] & 7) != 0)

static bool isValidName(const char *const name, const size_t maxLength) {
    if ((uintptr_t)name + maxLength < (uintptr_t)name) {
        // Wrapped around address space.
        return false;
    }

    char buffer[maxLength];
    size_t length = bsg_ksmachcopyMaxPossibleMem(name, buffer, maxLength);
    if (length == 0 || !VALID_NAME_START_CHAR(name[0])) {
        return false;
    }
    for (size_t i = 1; i < length; i++) {
        unlikely_if(!VALID_NAME_CHAR(name[i])) {
            if (name[i] == 0) {
                return true;
            }
            return false;
        }
    }
    return false;
}

static bool isValidIvarType(const char *const type) {
    char buffer[100];
    const size_t maxLength = sizeof(buffer);

    if ((uintptr_t)type + maxLength < (uintptr_t)type) {
        // Wrapped around address space.
        return false;
    }

    size_t length = bsg_ksmachcopyMaxPossibleMem(type, buffer, maxLength);
    if (length == 0 || !VALID_TYPE_CHAR(type[0])) {
        return false;
    }
    for (size_t i = 0; i < length; i++) {
        unlikely_if(!VALID_TYPE_CHAR(type[i])) {
            if (type[i] == 0) {
                return true;
            }
        }
    }
    return false;
}

static bool containsValidROData(const void *const classPtr) {
    struct class_t class;
    struct class_rw_t rw;
    struct class_ro_t ro;
    if (bsg_ksmachcopyMem(classPtr, &class, sizeof(class)) != KERN_SUCCESS) {
        return false;
    }
    if (bsg_ksmachcopyMem(getClassRW(&class), &rw, sizeof(rw)) !=
        KERN_SUCCESS) {
        return false;
    }
    if (bsg_ksmachcopyMem(rw.ro, &ro, sizeof(ro)) != KERN_SUCCESS) {
        return false;
    }
    return true;
}

static bool containsValidIvarData(const void *const classPtr) {
    const struct class_ro_t *ro = getClassRO(classPtr);
    const struct ivar_list_t *ivars = ro->ivars;
    if (ivars == NULL) {
        return true;
    }

    struct ivar_list_t ivarsBuffer;
    if (bsg_ksmachcopyMem(ivars, &ivarsBuffer, sizeof(ivarsBuffer)) !=
        KERN_SUCCESS) {
        return false;
    }

    if (ivars->count > 0) {
        struct ivar_t ivar;
        uint8_t *ivarPtr = (uint8_t *)(&ivars->first) + ivars->entsizeAndFlags;
        for (uint32_t i = 1; i < ivarsBuffer.count; i++) {
            if (bsg_ksmachcopyMem(ivarPtr, &ivar, sizeof(ivar)) !=
                KERN_SUCCESS) {
                return false;
            }
            uintptr_t offset;
            if (bsg_ksmachcopyMem(ivar.offset, &offset, sizeof(offset)) !=
                KERN_SUCCESS) {
                return false;
            }
            if (!isValidName(ivar.name, kMaxNameLength)) {
                return false;
            }
            if (!isValidIvarType(ivar.type)) {
                return false;
            }
            ivarPtr += ivars->entsizeAndFlags;
        }
    }
    return true;
}

static bool containsValidClassName(const void *const classPtr) {
    const struct class_ro_t *ro = getClassRO(classPtr);
    return isValidName(ro->name, kMaxNameLength);
}

//======================================================================
#pragma mark - Basic Objective-C Queries -
//======================================================================

const void *bsg_ksobjc_isaPointer(const void *const objectOrClassPtr) {
    return bsg_getIsaPointer(objectOrClassPtr);
}

const void *bsg_ksobjc_superClass(const void *const classPtr) {
    return getSuperClass(classPtr);
}

bool bsg_ksobjc_isMetaClass(const void *const classPtr) {
    return isMetaClass(classPtr);
}

bool bsg_ksobjc_isRootClass(const void *const classPtr) {
    return isRootClass(classPtr);
}

const char *bsg_ksobjc_className(const void *classPtr) {
    return getClassName(classPtr);
}

const char *bsg_ksobjc_objectClassName(const void *objectPtr) {
    if (bsg_isTaggedPointer(objectPtr)) {
        if (isValidTaggedPointer(objectPtr)) {
            const ClassData *class = getClassDataFromTaggedPointer(objectPtr);
            return class->name;
        }
        return NULL;
    }
    const void *isaPtr = bsg_getIsaPointer(objectPtr);
    return getClassName(isaPtr);
}

bool bsg_ksobjc_isClassNamed(const void *const classPtr,
                             const char *const className) {
    const char *name = getClassName(classPtr);
    if (name == NULL || className == NULL) {
        return false;
    }
    return strcmp(name, className) == 0;
}

bool bsg_ksobjc_isKindOfClass(const void *const classPtr,
                              const char *const className) {
    if (className == NULL) {
        return false;
    }

    const struct class_t *class = (const struct class_t *)classPtr;

    for (int i = 0; i < 20; i++) {
        const char *name = getClassName(class);
        if (name == NULL) {
            return false;
        }
        if (strcmp(className, name) == 0) {
            return true;
        }
        class = class->superclass;
        if (!containsValidROData(class)) {
            return false;
        }
    }
    return false;
}

const void *bsg_ksobjc_baseClass(const void *const classPtr) {
    const struct class_t *superClass = classPtr;
    const struct class_t *subClass = classPtr;

    for (int i = 0; i < 20; i++) {
        if (isRootClass(superClass)) {
            return subClass;
        }
        subClass = superClass;
        superClass = superClass->superclass;
        if (!containsValidROData(superClass)) {
            return NULL;
        }
    }
    return NULL;
}

size_t bsg_ksobjc_ivarCount(const void *const classPtr) {
    const struct ivar_list_t *ivars = getClassRO(classPtr)->ivars;
    if (ivars == NULL) {
        return 0;
    }
    return ivars->count;
}

size_t bsg_ksobjc_ivarList(const void *const classPtr, BSG_KSObjCIvar *dstIvars,
                           size_t ivarsCount) {
    if (dstIvars == NULL) {
        return 0;
    }

    size_t count = bsg_ksobjc_ivarCount(classPtr);
    if (count == 0) {
        return 0;
    }

    if (ivarsCount < count) {
        count = ivarsCount;
    }
    const struct ivar_list_t *srcIvars = getClassRO(classPtr)->ivars;
    uintptr_t srcPtr = (uintptr_t)&srcIvars->first;
    const struct ivar_t *src = (void *)srcPtr;
    for (size_t i = 0; i < count; i++) {
        BSG_KSObjCIvar *dst = &dstIvars[i];
        dst->name = src->name;
        dst->type = src->type;
        dst->index = i;
        srcPtr += srcIvars->entsizeAndFlags;
        src = (void *)srcPtr;
    }
    return count;
}

bool bsg_ksobjc_ivarNamed(const void *const classPtr, const char *name,
                          BSG_KSObjCIvar *dst) {
    if (name == NULL) {
        return false;
    }
    const struct ivar_list_t *ivars = getClassRO(classPtr)->ivars;
    uintptr_t ivarPtr = (uintptr_t)&ivars->first;
    const struct ivar_t *ivar = (void *)ivarPtr;
    for (size_t i = 0; i < ivars->count; i++) {
        if (ivar->name != NULL && strcmp(name, ivar->name) == 0) {
            dst->name = ivar->name;
            dst->type = ivar->type;
            dst->index = i;
            return true;
        }
        ivarPtr += ivars->entsizeAndFlags;
        ivar = (void *)ivarPtr;
    }
    return false;
}

bool bsg_ksobjc_ivarValue(const void *const objectPtr, size_t ivarIndex,
                          void *dst) {
    if (bsg_isTaggedPointer(objectPtr)) {
        // Naively assume they want "value".
        if (bsg_isTaggedPointerNSDate(objectPtr)) {
            CFTimeInterval value = extractTaggedNSDate(objectPtr);
            memcpy(dst, &value, sizeof(value));
            return true;
        }
        if (bsg_isTaggedPointerNSNumber(objectPtr)) {
            // TODO: Correct to assume 64-bit signed int? What does the actual
            // ivar say?
            int64_t value = extractTaggedNSNumber(objectPtr);
            memcpy(dst, &value, sizeof(value));
            return true;
        }
        return false;
    }

    const void *const classPtr = bsg_getIsaPointer(objectPtr);
    const struct ivar_list_t *ivars = getClassRO(classPtr)->ivars;
    if (ivarIndex >= ivars->count) {
        return false;
    }
    uintptr_t ivarPtr = (uintptr_t)&ivars->first;
    const struct ivar_t *ivar =
        (void *)(ivarPtr + ivars->entsizeAndFlags * ivarIndex);

    uintptr_t valuePtr = (uintptr_t)objectPtr + (uintptr_t)*ivar->offset;
    if (bsg_ksmachcopyMem((void *)valuePtr, dst, ivar->size) != KERN_SUCCESS) {
        return false;
    }
    return true;
}

uintptr_t bsg_ksobjc_taggedPointerPayload(const void *taggedObjectPtr) {
    return bsg_getTaggedPayload(taggedObjectPtr);
}

static inline bool isBlockClass(const void *class) {
    const void *baseClass = bsg_ksobjc_baseClass(class);
    if (baseClass == NULL) {
        return false;
    }
    const char *name = getClassName(baseClass);
    if (name == NULL) {
        return false;
    }
    return strcmp(name, bsg_g_blockBaseClassName) == 0;
}

BSG_KSObjCType bsg_ksobjc_objectType(const void *objectOrClassPtr) {
    if (objectOrClassPtr == NULL) {
        return BSG_KSObjCTypeUnknown;
    }

    if (bsg_isTaggedPointer(objectOrClassPtr)) {
        return BSG_KSObjCTypeObject;
    }

    if (!isValidObject(objectOrClassPtr)) {
        return BSG_KSObjCTypeUnknown;
    }

    if (!hasValidISAPointer(objectOrClassPtr)) {
        return BSG_KSObjCTypeUnknown;
    }

    const struct class_t *isa = bsg_getIsaPointer(objectOrClassPtr);

    if (!containsValidROData(isa)) {
        return BSG_KSObjCTypeUnknown;
    }
    if (!containsValidClassName(isa)) {
        return BSG_KSObjCTypeUnknown;
    }

    if (isBlockClass(isa)) {
        return BSG_KSObjCTypeBlock;
    }
    if (!isMetaClass(isa)) {
        return BSG_KSObjCTypeObject;
    }

    if (!containsValidIvarData(isa)) {
        return BSG_KSObjCTypeUnknown;
    }
    if (!containsValidClassName(isa)) {
        return BSG_KSObjCTypeUnknown;
    }

    return BSG_KSObjCTypeClass;
}

//======================================================================
#pragma mark - Unknown Object -
//======================================================================

static bool objectIsValid(__unused const void *object) {
    // If it passed bsg_ksobjc_objectType, it's been validated as much as
    // possible.
    return true;
}

static bool taggedObjectIsValid(const void *object) {
    return isValidTaggedPointer(object);
}

static size_t objectDescription(const void *object, char *buffer,
                                size_t bufferLength) {
    const void *class = bsg_getIsaPointer(object);
    const char *name = getClassName(class);
    uintptr_t objPointer = (uintptr_t)object;
    const char *fmt = sizeof(uintptr_t) == sizeof(uint32_t) ? "<%s: 0x%08x>"
                                                            : "<%s: 0x%016x>";
    return stringPrintf(buffer, bufferLength, fmt, name, objPointer);
}

static size_t taggedObjectDescription(const void *object, char *buffer,
                                      size_t bufferLength) {
    const ClassData *data = getClassDataFromTaggedPointer(object);
    const char *name = data->name;
    uintptr_t objPointer = (uintptr_t)object;
    const char *fmt = sizeof(uintptr_t) == sizeof(uint32_t) ? "<%s: 0x%08x>"
                                                            : "<%s: 0x%016x>";
    return stringPrintf(buffer, bufferLength, fmt, name, objPointer);
}

//======================================================================
#pragma mark - NSString -
//======================================================================

static inline const char *stringStart(const struct __CFString *str) {
    return (const char *)__CFStrContents(str) +
           (__CFStrHasLengthByte(str) ? 1 : 0);
}

static bool stringIsValid(const void *const stringPtr) {
    const struct __CFString *string = stringPtr;
    struct __CFString temp;
    uint8_t oneByte;
    CFIndex length = -1;
    if (bsg_ksmachcopyMem(string, &temp, sizeof(string->base)) !=
        KERN_SUCCESS) {
        return false;
    }

    if (__CFStrIsInline(string)) {
        if (bsg_ksmachcopyMem(&string->variants.inline1, &temp,
                              sizeof(string->variants.inline1)) !=
            KERN_SUCCESS) {
            return false;
        }
        length = string->variants.inline1.length;
    } else if (__CFStrIsMutable(string)) {
        if (bsg_ksmachcopyMem(&string->variants.notInlineMutable, &temp,
                              sizeof(string->variants.notInlineMutable)) !=
            KERN_SUCCESS) {
            return false;
        }
        length = string->variants.notInlineMutable.length;
    } else if (!__CFStrHasLengthByte(string)) {
        if (bsg_ksmachcopyMem(&string->variants.notInlineImmutable1, &temp,
                              sizeof(string->variants.notInlineImmutable1)) !=
            KERN_SUCCESS) {
            return false;
        }
        length = string->variants.notInlineImmutable1.length;
    } else {
        if (bsg_ksmachcopyMem(&string->variants.notInlineImmutable2, &temp,
                              sizeof(string->variants.notInlineImmutable2)) !=
            KERN_SUCCESS) {
            return false;
        }
        if (bsg_ksmachcopyMem(__CFStrContents(string), &oneByte,
                              sizeof(oneByte)) != KERN_SUCCESS) {
            return false;
        }
        length = oneByte;
    }

    if (length < 0) {
        return false;
    } else if (length > 0) {
        if (bsg_ksmachcopyMem(stringStart(string), &oneByte, sizeof(oneByte)) !=
            KERN_SUCCESS) {
            return false;
        }
    }
    return true;
}

size_t bsg_ksobjc_stringLength(const void *const stringPtr) {
    if (bsg_isTaggedPointer(stringPtr) &&
        bsg_isTaggedPointerNSString(stringPtr)) {
        return getTaggedNSStringLength(stringPtr);
    }

    const struct __CFString *string = stringPtr;

    if (__CFStrHasExplicitLength(string)) {
        if (__CFStrIsInline(string)) {
            return (size_t)string->variants.inline1.length;
        } else {
            return (size_t)string->variants.notInlineImmutable1.length;
        }
    } else {
        return (size_t)(*((uint8_t *)__CFStrContents(string)));
    }
}

#define kUTF16_LeadSurrogateStart 0xd800u
#define kUTF16_LeadSurrogateEnd 0xdbffu
#define kUTF16_TailSurrogateStart 0xdc00u
#define kUTF16_TailSurrogateEnd 0xdfffu
#define kUTF16_FirstSupplementaryPlane 0x10000u

size_t bsg_ksobjc_i_copyAndConvertUTF16StringToUTF8(const void *const src,
                                                    void *const dst,
                                                    size_t charCount,
                                                    size_t maxByteCount) {
    const uint16_t *pSrc = src;
    uint8_t *pDst = dst;
    const uint8_t *const pDstEnd =
        pDst + maxByteCount - 1; // Leave room for null termination.
    for (size_t charsRemaining = charCount;
         charsRemaining > 0 && pDst < pDstEnd; charsRemaining--) {
        // Decode UTF-16
        uint32_t character = 0;
        uint16_t leadSurrogate = *pSrc++;
        likely_if(leadSurrogate < kUTF16_LeadSurrogateStart ||
                  leadSurrogate > kUTF16_TailSurrogateEnd) {
            character = leadSurrogate;
        }
        else if (leadSurrogate > kUTF16_LeadSurrogateEnd) {
            // Inverted surrogate
            *((uint8_t *)dst) = 0;
            return 0;
        }
        else {
            uint16_t tailSurrogate = *pSrc++;
            if (tailSurrogate < kUTF16_TailSurrogateStart ||
                tailSurrogate > kUTF16_TailSurrogateEnd) {
                // Invalid tail surrogate
                *((uint8_t *)dst) = 0;
                return 0;
            }
            character = ((leadSurrogate - kUTF16_LeadSurrogateStart) << 10) +
                        (tailSurrogate - kUTF16_TailSurrogateStart);
            character += kUTF16_FirstSupplementaryPlane;
            charsRemaining--;
        }

        // Encode UTF-8
        likely_if(character <= 0x7f) { *pDst++ = (uint8_t)character; }
        else if (character <= 0x7ff) {
            if (pDstEnd - pDst >= 2) {
                *pDst++ = (uint8_t)(0xc0 | (character >> 6));
                *pDst++ = (uint8_t)(0x80 | (character & 0x3f));
            } else {
                break;
            }
        }
        else if (character <= 0xffff) {
            if (pDstEnd - pDst >= 3) {
                *pDst++ = (uint8_t)(0xe0 | (character >> 12));
                *pDst++ = (uint8_t)(0x80 | ((character >> 6) & 0x3f));
                *pDst++ = (uint8_t)(0x80 | (character & 0x3f));
            } else {
                break;
            }
        }
        // RFC3629 restricts UTF-8 to end at 0x10ffff.
        else if (character <= 0x10ffff) {
            if (pDstEnd - pDst >= 4) {
                *pDst++ = (uint8_t)(0xf0 | (character >> 18));
                *pDst++ = (uint8_t)(0x80 | ((character >> 12) & 0x3f));
                *pDst++ = (uint8_t)(0x80 | ((character >> 6) & 0x3f));
                *pDst++ = (uint8_t)(0x80 | (character & 0x3f));
            } else {
                break;
            }
        }
        else {
            // Invalid unicode.
            *((uint8_t *)dst) = 0;
            return 0;
        }
    }

    // Null terminate and return.
    *pDst = 0;
    return (size_t)(pDst - (uint8_t *)dst);
}

size_t bsg_ksobjc_i_copy8BitString(const void *const src, void *const dst,
                                   size_t charCount, size_t maxByteCount) {
    unlikely_if(maxByteCount == 0) { return 0; }
    unlikely_if(charCount == 0) {
        *((uint8_t *)dst) = 0;
        return 0;
    }

    unlikely_if(charCount >= maxByteCount) { charCount = maxByteCount - 1; }
    unlikely_if(bsg_ksmachcopyMem(src, dst, charCount) != KERN_SUCCESS) {
        *((uint8_t *)dst) = 0;
        return 0;
    }
    uint8_t *charDst = dst;
    charDst[charCount] = 0;
    return charCount;
}

size_t bsg_ksobjc_copyStringContents(const void *stringPtr, char *dst,
                                     size_t maxByteCount) {
    if (bsg_isTaggedPointer(stringPtr) &&
        bsg_isTaggedPointerNSString(stringPtr)) {
        return extractTaggedNSString(stringPtr, dst, maxByteCount);
    }
    const struct __CFString *string = stringPtr;
    size_t charCount = bsg_ksobjc_stringLength(string);

    const char *src = stringStart(string);
    if (__CFStrIsUnicode(string)) {
        return bsg_ksobjc_i_copyAndConvertUTF16StringToUTF8(src, dst, charCount,
                                                            maxByteCount);
    }

    return bsg_ksobjc_i_copy8BitString(src, dst, charCount, maxByteCount);
}

static size_t stringDescription(const void *object, char *buffer,
                                size_t bufferLength) {
    char *pBuffer = buffer;
    char *pEnd = buffer + bufferLength;

    pBuffer += objectDescription(object, pBuffer, (size_t)(pEnd - pBuffer));
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), ": \"");
    pBuffer += bsg_ksobjc_copyStringContents(object, pBuffer,
                                             (size_t)(pEnd - pBuffer));
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), "\"");

    return (size_t)(pBuffer - buffer);
}

static bool taggedStringIsValid(const void *const object) {
    return isValidTaggedPointer(object) && bsg_isTaggedPointerNSString(object);
}

static size_t taggedStringDescription(const void *object, char *buffer,
                                      __unused size_t bufferLength) {
    return extractTaggedNSString(object, buffer, bufferLength);
}

//======================================================================
#pragma mark - NSURL -
//======================================================================

static bool urlIsValid(const void *const urlPtr) {
    struct __CFURL url;
    if (bsg_ksmachcopyMem(urlPtr, &url, sizeof(url)) != KERN_SUCCESS) {
        return false;
    }
    return stringIsValid(url._string);
}

size_t bsg_ksobjc_copyURLContents(const void *const urlPtr, char *dst,
                                  size_t maxLength) {
    const struct __CFURL *url = urlPtr;
    return bsg_ksobjc_copyStringContents(url->_string, dst, maxLength);
}

static size_t urlDescription(const void *object, char *buffer,
                             size_t bufferLength) {
    char *pBuffer = buffer;
    char *pEnd = buffer + bufferLength;

    pBuffer += objectDescription(object, pBuffer, (size_t)(pEnd - pBuffer));
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), ": \"");
    pBuffer +=
        bsg_ksobjc_copyURLContents(object, pBuffer, (size_t)(pEnd - pBuffer));
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), "\"");

    return (size_t)(pBuffer - buffer);
}

//======================================================================
#pragma mark - NSDate -
//======================================================================

static bool dateIsValid(const void *const datePtr) {
    struct __CFDate temp;
    return bsg_ksmachcopyMem(datePtr, &temp, sizeof(temp)) == KERN_SUCCESS;
}

CFAbsoluteTime bsg_ksobjc_dateContents(const void *const datePtr) {
    if (isValidTaggedPointer(datePtr)) {
        return extractTaggedNSDate(datePtr);
    }
    const struct __CFDate *date = datePtr;
    return date->_time;
}

static size_t dateDescription(const void *object, char *buffer,
                              size_t bufferLength) {
    char *pBuffer = buffer;
    char *pEnd = buffer + bufferLength;

    CFAbsoluteTime time = bsg_ksobjc_dateContents(object);
    pBuffer += objectDescription(object, pBuffer, (size_t)(pEnd - pBuffer));
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), ": %f", time);

    return (size_t)(pBuffer - buffer);
}

static bool taggedDateIsValid(const void *const datePtr) {
    return isValidTaggedPointer(datePtr) && bsg_isTaggedPointerNSDate(datePtr);
}

static size_t taggedDateDescription(const void *object, char *buffer,
                                    size_t bufferLength) {
    char *pBuffer = buffer;
    char *pEnd = buffer + bufferLength;

    CFAbsoluteTime time = extractTaggedNSDate(object);
    pBuffer +=
        taggedObjectDescription(object, pBuffer, (size_t)(pEnd - pBuffer));
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), ": %f", time);

    return (size_t)(pBuffer - buffer);
}

//======================================================================
#pragma mark - NSNumber -
//======================================================================

#define NSNUMBER_CASE(CFTYPE, RETURN_TYPE, CAST_TYPE, DATA)                    \
    case CFTYPE: {                                                             \
        RETURN_TYPE result;                                                    \
        memcpy(&result, DATA, sizeof(result));                                 \
        return (CAST_TYPE)result;                                              \
    }

#define EXTRACT_AND_RETURN_NSNUMBER(OBJECT, RETURN_TYPE)                       \
    if (isValidTaggedPointer(object)) {                                        \
        return extractTaggedNSNumber(object);                                  \
    }                                                                          \
    const struct __CFNumber *number = OBJECT;                                  \
    CFNumberType cftype = CFNumberGetType((CFNumberRef)OBJECT);                \
    const void *data = &(number->_pad);                                        \
    switch (cftype) {                                                          \
        NSNUMBER_CASE(kCFNumberSInt8Type, int8_t, RETURN_TYPE, data)           \
        NSNUMBER_CASE(kCFNumberSInt16Type, int16_t, RETURN_TYPE, data)         \
        NSNUMBER_CASE(kCFNumberSInt32Type, int32_t, RETURN_TYPE, data)         \
        NSNUMBER_CASE(kCFNumberSInt64Type, int64_t, RETURN_TYPE, data)         \
        NSNUMBER_CASE(kCFNumberFloat32Type, Float32, RETURN_TYPE, data)        \
        NSNUMBER_CASE(kCFNumberFloat64Type, Float64, RETURN_TYPE, data)        \
        NSNUMBER_CASE(kCFNumberCharType, char, RETURN_TYPE, data)              \
        NSNUMBER_CASE(kCFNumberShortType, short, RETURN_TYPE, data)            \
        NSNUMBER_CASE(kCFNumberIntType, int, RETURN_TYPE, data)                \
        NSNUMBER_CASE(kCFNumberLongType, long, RETURN_TYPE, data)              \
        NSNUMBER_CASE(kCFNumberLongLongType, long long, RETURN_TYPE, data)     \
        NSNUMBER_CASE(kCFNumberFloatType, float, RETURN_TYPE, data)            \
        NSNUMBER_CASE(kCFNumberDoubleType, double, RETURN_TYPE, data)          \
        NSNUMBER_CASE(kCFNumberCFIndexType, CFIndex, RETURN_TYPE, data)        \
        NSNUMBER_CASE(kCFNumberNSIntegerType, NSInteger, RETURN_TYPE, data)    \
        NSNUMBER_CASE(kCFNumberCGFloatType, CGFloat, RETURN_TYPE, data)        \
    }

Float64 bsg_ksobjc_numberAsFloat(const void *object) {
    EXTRACT_AND_RETURN_NSNUMBER(object, Float64);
    return NAN;
}

int64_t bsg_ksobjc_numberAsInteger(const void *object) {
    EXTRACT_AND_RETURN_NSNUMBER(object, int64_t);
    return 0;
}

bool bsg_ksobjc_numberIsFloat(const void *object) {
    return CFNumberIsFloatType((CFNumberRef)object);
}

static bool numberIsValid(const void *const datePtr) {
    struct __CFNumber temp;
    return bsg_ksmachcopyMem(datePtr, &temp, sizeof(temp)) == KERN_SUCCESS;
}

static size_t numberDescription(const void *object, char *buffer,
                                size_t bufferLength) {
    char *pBuffer = buffer;
    char *pEnd = buffer + bufferLength;

    pBuffer += objectDescription(object, pBuffer, (size_t)(pEnd - pBuffer));

    if (bsg_ksobjc_numberIsFloat(object)) {
        int64_t value = bsg_ksobjc_numberAsInteger(object);
        pBuffer +=
            stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), ": %ld", value);
    } else {
        Float64 value = bsg_ksobjc_numberAsFloat(object);
        pBuffer +=
            stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), ": %lf", value);
    }

    return (size_t)(pBuffer - buffer);
}

static bool taggedNumberIsValid(const void *const object) {
    return isValidTaggedPointer(object) && bsg_isTaggedPointerNSNumber(object);
}

static size_t taggedNumberDescription(const void *object, char *buffer,
                                      size_t bufferLength) {
    char *pBuffer = buffer;
    char *pEnd = buffer + bufferLength;

    int64_t value = extractTaggedNSNumber(object);
    pBuffer +=
        taggedObjectDescription(object, pBuffer, (size_t)(pEnd - pBuffer));
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), ": %ld", value);

    return (size_t)(pBuffer - buffer);
}

//======================================================================
#pragma mark - NSArray -
//======================================================================

struct NSArray {
    struct {
        void *isa;
        CFIndex count;
        id firstEntry;
    } basic;
};

static inline bool nsarrayIsMutable(const void *const arrayPtr) {
    return getClassDataFromObject(arrayPtr)->isMutable;
}

static inline bool nsarrayIsValid(const void *const arrayPtr) {
    struct NSArray temp;
    if (bsg_ksmachcopyMem(arrayPtr, &temp, sizeof(temp.basic)) !=
        KERN_SUCCESS) {
        return false;
    }
    return true;
}

static inline size_t nsarrayCount(const void *const arrayPtr) {
    const struct NSArray *array = arrayPtr;
    return array->basic.count < 0 ? 0 : (size_t)array->basic.count;
}

static size_t nsarrayContents(const void *const arrayPtr, uintptr_t *contents,
                              size_t count) {
    const struct NSArray *array = arrayPtr;

    if (array->basic.count < (CFIndex)count) {
        if (array->basic.count <= 0) {
            return 0;
        }
        count = (size_t)array->basic.count;
    }
    // TODO: implement this (requires bit-field unpacking) in ksobj_ivarValue
    if (nsarrayIsMutable(arrayPtr)) {
        return 0;
    }

    if (bsg_ksmachcopyMem(&array->basic.firstEntry, contents,
                          sizeof(*contents) * count) != KERN_SUCCESS) {
        return 0;
    }
    return count;
}

static inline bool cfarrayIsValid(const void *const arrayPtr) {
    struct __CFArray temp;
    if (bsg_ksmachcopyMem(arrayPtr, &temp, sizeof(temp)) != KERN_SUCCESS) {
        return false;
    }
    const struct __CFArray *array = arrayPtr;
    if (__CFArrayGetType(array) == __kCFArrayDeque) {
        if (array->_store != NULL) {
            struct __CFArrayDeque deque;
            if (bsg_ksmachcopyMem(array->_store, &deque, sizeof(deque)) !=
                KERN_SUCCESS) {
                return false;
            }
        }
    }
    return true;
}

static inline const void *cfarrayData(const void *const arrayPtr) {
    return __CFArrayGetBucketsPtr(arrayPtr);
}

static inline size_t cfarrayCount(const void *const arrayPtr) {
    const struct __CFArray *array = arrayPtr;
    return array->_count < 0 ? 0 : (size_t)array->_count;
}

static size_t cfarrayContents(const void *const arrayPtr, uintptr_t *contents,
                              size_t count) {
    const struct __CFArray *array = arrayPtr;
    if (array->_count < (CFIndex)count) {
        if (array->_count <= 0) {
            return 0;
        }
        count = (size_t)array->_count;
    }

    const void *firstEntry = cfarrayData(array);
    if (bsg_ksmachcopyMem(firstEntry, contents, sizeof(*contents) * count) !=
        KERN_SUCCESS) {
        return 0;
    }
    return count;
}

static bool isCFArray(const void *const arrayPtr) {
    const ClassData *data = getClassDataFromObject(arrayPtr);
    return data->subtype == ClassSubtypeCFArray;
}

size_t bsg_ksobjc_arrayCount(const void *const arrayPtr) {
    if (isCFArray(arrayPtr)) {
        return cfarrayCount(arrayPtr);
    }
    return nsarrayCount(arrayPtr);
}

size_t bsg_ksobjc_arrayContents(const void *const arrayPtr, uintptr_t *contents,
                                size_t count) {
    if (isCFArray(arrayPtr)) {
        return cfarrayContents(arrayPtr, contents, count);
    }
    return nsarrayContents(arrayPtr, contents, count);
}

bool arrayIsValid(const void *object) {
    if (isCFArray(object)) {
        return cfarrayIsValid(object);
    }
    return nsarrayIsValid(object);
}

static size_t arrayDescription(const void *object, char *buffer,
                               size_t bufferLength) {
    char *pBuffer = buffer;
    char *pEnd = buffer + bufferLength;

    pBuffer += objectDescription(object, pBuffer, (size_t)(pEnd - pBuffer));
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), ": [");

    if (pBuffer < pEnd - 1 && bsg_ksobjc_arrayCount(object) > 0) {
        uintptr_t contents = 0;
        if (bsg_ksobjc_arrayContents(object, &contents, 1) == 1) {
            pBuffer += bsg_ksobjc_getDescription((void *)contents, pBuffer,
                                                 (size_t)(pEnd - pBuffer));
        }
    }
    pBuffer += stringPrintf(pBuffer, (size_t)(pEnd - pBuffer), "]");

    return (size_t)(pBuffer - buffer);
}

//======================================================================
#pragma mark - NSDictionary (BROKEN) -
//======================================================================

bool bsg_ksobjc_dictionaryFirstEntry(const void *dict, uintptr_t *key,
                                     uintptr_t *value) {
    // TODO: This is broken.

    // Ensure memory is valid.
    struct __CFBasicHash copy;
    kern_return_t kr = KERN_SUCCESS;
    if ((kr = bsg_ksmachcopyMem(dict, &copy, sizeof(copy))) != KERN_SUCCESS) {
        return false;
    }

    struct __CFBasicHash *ht = (struct __CFBasicHash *)dict;
    uintptr_t *keys = (uintptr_t *)ht->pointers + ht->bits.keys_offset;
    uintptr_t *values = (uintptr_t *)ht->pointers;

    // Dereference key and value pointers.
    if ((kr = bsg_ksmachcopyMem(keys, &keys, sizeof(keys))) != KERN_SUCCESS) {
        return false;
    }

    if ((kr = bsg_ksmachcopyMem(values, &values, sizeof(values))) !=
        KERN_SUCCESS) {
        return false;
    }

    // Copy to destination.
    if ((kr = bsg_ksmachcopyMem(keys, key, sizeof(*key))) != KERN_SUCCESS) {
        return false;
    }
    if ((kr = bsg_ksmachcopyMem(values, value, sizeof(*value))) !=
        KERN_SUCCESS) {
        return false;
    }
    return true;
}

// kern_return_t bsg_ksobjc_dictionaryContents(const void* dict, uintptr_t*
// keys, uintptr_t* values, CFIndex* count)
//{
//    struct CFBasicHash copy;
//    void* pointers[100];
//
//    kern_return_t kr = KERN_SUCCESS;
//    if((kr = bsg_ksmachcopyMem(dict, &copy, sizeof(copy))) != KERN_SUCCESS)
//    {
//        return kr;
//    }
//
//    struct CFBasicHash* ht = (struct CFBasicHash*)dict;
//    size_t values_offset = 0;
//    size_t keys_offset = copy.bits.keys_offset;
//    if((kr = bsg_ksmachcopyMem(&ht->pointers, pointers, sizeof(*pointers) *
//    keys_offset)) != KERN_SUCCESS)
//    {
//        return kr;
//    }
//
//
//
//    return kr;
//}

size_t bsg_ksobjc_dictionaryCount(const void *dict) {
// TODO: Implement me
#pragma unused(dict)
    return 0;
}

//======================================================================
#pragma mark - General Queries -
//======================================================================

size_t bsg_ksobjc_getDescription(void *object, char *buffer,
                                 size_t bufferLength) {
    const ClassData *data = getClassDataFromObject(object);
    return data->description(object, buffer, bufferLength);
}

void *bsg_ksobjc_i_objectReferencedByString(const char *string) {
    uint64_t address = 0;
    if (bsg_ksstring_extractHexValue(string, strlen(string), &address)) {
        return (void *)address;
    }
    return NULL;
}

bool bsg_ksobjc_bsg_isTaggedPointer(const void *const pointer) {
    return bsg_isTaggedPointer(pointer);
}

bool bsg_ksobjc_isValidTaggedPointer(const void *const pointer) {
    return isValidTaggedPointer(pointer);
}

bool bsg_ksobjc_isValidObject(const void *object) {
    if (!isValidObject(object)) {
        return false;
    }
    const ClassData *data = getClassDataFromObject(object);
    return data->isValidObject(object);
}

BSG_KSObjCClassType bsg_ksobjc_objectClassType(const void *object) {
    const ClassData *data = getClassDataFromObject(object);
    return data->type;
}

void bsg_ksobjc_init(void) {}

//__NSArrayReversed
//__NSCFBoolean
//__NSCFDictionary
//__NSCFError
//__NSCFNumber
//__NSCFSet
//__NSCFString
//__NSDate
//__NSDictionaryI
//__NSDictionaryM
//__NSOrderedSetArrayProxy
//__NSOrderedSetI
//__NSOrderedSetM
//__NSOrderedSetReversed
//__NSOrderedSetSetProxy
//__NSPlaceholderArray
//__NSPlaceholderDate
//__NSPlaceholderDictionary
//__NSPlaceholderOrderedSet
//__NSPlaceholderSet
//__NSSetI
//__NSSetM
// NSArray
// NSCFArray
// NSCFBoolean
// NSCFDictionary
// NSCFError
// NSCFNumber
// NSCFSet
// NSCheapMutableString
// NSClassicHashTable
// NSClassicMapTable
// SConcreteHashTable
// NSConcreteMapTable
// NSConcreteValue
// NSDate
// NSDecimalNumber
// NSDecimalNumberPlaceholder
// NSDictionary
// NSError
// NSException
// NSHashTable
// NSMutableArray
// NSMutableDictionary
// NSMutableIndexSet
// NSMutableOrderedSet
// NSMutableRLEArray
// NSMutableSet
// NSMutableString
// NSMutableStringProxy
// NSNumber
// NSOrderedSet
// NSPlaceholderMutableString
// NSPlaceholderNumber
// NSPlaceholderString
// NSRLEArray
// NSSet
// NSSimpleCString
// NSString
// NSURL
