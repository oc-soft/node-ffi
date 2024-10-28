#ifndef __CODE_OBJECT_H__
#define __CODE_OBJECT_H__
#include "nan.h"

class callback_info;

namespace ffi {


/**
 * It keeps ffi callback info
 */
class CodeObject : Nan::ObjectWrap
{
    /**
     * callback info
     */
    callback_info* callbackInfo;
public:
    /**
     * constructor
     */
    CodeObject(
        callback_info* cb);

    /**
     * destructor
     */
    virtual ~CodeObject();
    
    
    /**
     * attach this object to a value
     */
    void AttachTo(v8::Local<v8::Object>& value);
};
}
// vi: se ts=4 sw=4 et:
#endif
