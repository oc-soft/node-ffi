#ifndef __CODE_OBJECT_H__
#define __CODE_OBJECT_H__
#include "nan.h"
#ifdef __cplusplus

namespace node_ffi {

class Closure;
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
    Closure* callbackInfo;
public:
    /**
     * constructor
     */
    CodeObject(
        Closure* cb);

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
