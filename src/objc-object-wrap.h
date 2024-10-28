#ifndef __OBJC_OBJECT_WRAP_H__
#define __OBJC_OBJECT_WRAP_H__

#if __OBJC__ || __OBJC2__
 
#include <v8.h>
#include <objc/objc.h>
#include "nan.h"

/**
 * objective c object wrapper
 */
class ObjcObjectWrap : Nan::ObjectWrap {

    /**
     * objective c object
     */
    id objc = Nil;
    /**
     * constructor
     */
    ObjcObjectWrap(id objc = Nil);


public:
    /**
     * destructor
     */
    ~ObjcObjectWrap();

    static v8::MaybeLocal<v8::Object>
    New(
        v8::Isolate* isolate,
        id objc);
};

// vi: se ts=4 sw=4 et:
#endif
#endif
