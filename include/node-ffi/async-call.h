#ifndef __NODE_FFI_ASYNC_CALL_H__
#define __NODE_FFI_ASYNC_CALL_H__

#include <memory>
#include <ffi.h>
#include <nan.h>

#if __OBJC__ || __OBJC2__
#include <objc/objc.h>
#endif


#ifdef __cplusplus
namespace node_ffi {
/*
 * Class used to store stuff during async ffi_call() invokations.
 */

class AsyncCall {
  public:
    AsyncCall() = default;

    ~AsyncCall();
#if __OBJC__ || __OBJC2__
    bool
    HasErr() const;

    id
    GetErrRef() const;

    void
    SetErr(
        id err);

    MaybeLocal<v8::Object>
    GetErr(Isolate* isolate);
#endif

    void
    SetCif(
        ffi_cif* cif);

    ffi_cif*
    GetCif() const;

    void
    (*GetFn() const)(void);

    void
    SetFn(
        void (*fn)(void));

    void*
    GetRes() const;

    void
    SetRes(
        void* res);

    void**
    GetArgv() const;

    void
    SetArgv(
        void** argv);

    void
    SetCallback(
        std::unique_ptr<Nan::Callback>& callback);

    std::unique_ptr<Nan::Callback>&
    GetCallback();

    /**
     * register aynchronous functions
     */
    static NAN_MODULE_INIT(Register);

private:
#if __OBJC__ || __OBJC2__
    id err = Nil;
#endif
    ffi_cif *cif = nullptr;
    void (*fn)(void) = nullptr;
    void *res = nullptr;
    void **argv = nullptr;
    std::unique_ptr<Nan::Callback> callback;

    /**
     * asynchronous function entry point for javascript
     */
    static NAN_METHOD(FFICallAsync);
    /**
     * worker callback
     */
    static void AsyncFFICall(uv_work_t *req);
    /**
     * clean up function
     */
    static void FinishAsyncFFICall(uv_work_t *req);

};

}
#endif
#endif
