#ifndef __NODE_FFI_CALLBACK_H__
#define __NODE_FFI_CALLBACK_H__
#include <queue>
#include <ffi.h>
#include <uv.h>
#include <nan.h>

#ifdef __cplusplus



namespace node_ffi {
class AsyncCall;
class AsyncHandle;
class Closure;

class Callback {
    friend class AsyncCall;
    friend class AsyncHandle;
public:
    static NAN_MODULE_INIT(Initialize);

  protected:
    static void DispatchToV8(Closure *self, void *retval, void **parameters, bool dispatched = false);
    static void Invoke(ffi_cif *cif, void *retval, void **parameters, void *user_data);
    static NAN_METHOD(NewCallback);

  private:
    /**
     * decode Closure from code buffer object.
     */
    static Closure*
    DecodeCallbackInfo(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& codeBuffer);


};

}
#endif
#endif
