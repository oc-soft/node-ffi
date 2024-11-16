#ifndef __CODE_OBJECT_H__
#define __CODE_OBJECT_H__
#include "nan.h"
#ifdef __cplusplus
class callback_info;

namespace node_ffi {

class Callback;
/**
 * It keeps ffi callback info
 */
class CodeObject : public Nan::ObjectWrap
{
    friend class Callback;
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
#endif
// vi: se ts=4 sw=4 et:
#endif
