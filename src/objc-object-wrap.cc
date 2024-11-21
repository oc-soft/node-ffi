#include "node-ffi/objc-object-wrap.h"

#if __OBJC__ || __OBJC2__
namespace node_ffi {
/**
 * constructor
 */
ObjcObjectWrap::ObjcObjectWrap(id objc = Nil)
{
    [objc retain];
    this->objc = objc;
}

/**
 * destructor
 */
ObjcObjectWrap::~ObjcObjectWrap()
{
    if (objc) {
        [objc release];
    }
}

static v8::MaybeLocal<v8::Object>
ObjcObjectWrap::New(
    v8::Isolate* isolate,
    id objc)
{
    v8::MaybeLocal<v8::Object> result;

    ObjcObjectWrap* wrapObj;
    wrapObj = new (std::nothrow) ObjecObjectWrap(id);
    if (wrapObj) {
        result = v8::Object::New(isolate);
        if (result->IsObject()) {
            wrapObj->Wrap(result);   
        }
    }
    return result; 
}
}
// vi: se ts=4 sw=4 et:
#endif
