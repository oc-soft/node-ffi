#include "code-object.h"
#include "node-ffi/closure.h"

namespace node_ffi {

/**
 * constructor
 */
CodeObject::CodeObject(Closure* cb)
    : callbackInfo(cb)
{
}

/**
 * destructor
 */
CodeObject::~CodeObject()
{
    if (callbackInfo) {
        Closure::Free(callbackInfo);
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
