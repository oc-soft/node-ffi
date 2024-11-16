#ifndef __NODE_FFI_CALLBACK_H__
#define __NODE_FFI_CALLBACK_H__
#include <queue>
#include <ffi.h>
#include <uv.h>
#include <nan.h>

#ifdef __cplusplus


class ThreadedCallbackInvokation;
class callback_info;

namespace node_ffi {
class AsyncCall;

class Callback {
    friend class AsyncCall;
public:
    static NAN_MODULE_INIT(Initialize);
    static void WatcherCallback(uv_async_t *w, int revents);

  protected:
    static void DispatchToV8(callback_info *self, void *retval, void **parameters, bool dispatched = false);
    static void Invoke(ffi_cif *cif, void *retval, void **parameters, void *user_data);
    static NAN_METHOD(NewCallback);

  private:
#ifdef WIN32
    static DWORD g_threadID;
#else
    static uv_thread_t          g_mainthread;
#endif // WIN32
    static uv_mutex_t    g_queue_mutex;
    static std::queue<ThreadedCallbackInvokation *> g_queue;
    static uv_async_t         g_async;
    /**
     * decode callback_info from code buffer object.
     */
    static callback_info*
    DecodeCallbackInfo(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& codeBuffer);


};

}
#endif
#endif
