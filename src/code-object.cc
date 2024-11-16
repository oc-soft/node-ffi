#include "code-object.h"
#include "callback-info-i.h"

namespace node_ffi {

/**
 * constructor
 */
CodeObject::CodeObject(callback_info* cb)
    : callbackInfo(cb)
{
}

/**
 * destructor
 */
CodeObject::~CodeObject()
{
    if (callbackInfo) {
        callback_info::Free(callbackInfo);
        callbackInfo = nullptr;
    } 
}

/**
 * attach this object to a value
 */
void
CodeObject::AttachTo(v8::Local<v8::Object>& value)
{
    Wrap(value);
}

}
