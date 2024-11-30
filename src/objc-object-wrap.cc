#include "node-ffi/objc-object-wrap.h"

#if __OBJC__ || __OBJC2__
namespace node_ffi {
/**
 * constructor
 */
ObjcObjectWrap::ObjcObjectWrap(id objc)
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

v8::MaybeLocal<v8::Object>
ObjcObjectWrap::New(
    v8::Isolate* isolate,
    id objc)
{
    v8::MaybeLocal<v8::Object> result;

    ObjcObjectWrap* wrapObj;
    wrapObj = new (std::nothrow) ObjcObjectWrap(objc);
    if (wrapObj) {
        v8::Local<v8::ObjectTemplate> jsTemp;
        jsTemp = v8::ObjectTemplate::New(isolate);
        jsTemp->SetInternalFieldCount(1);
        result = jsTemp->NewInstance(isolate->GetCurrentContext());
        if (!result.IsEmpty()) {
            wrapObj->Wrap(result.ToLocalChecked());
        }
    }
    return result; 
}
}
// vi: se ts=4 sw=4 et:
#endif
