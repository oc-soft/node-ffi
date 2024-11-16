#ifndef __NODE_FFI_ASYNC_CALL_H__
#define __NODE_FFI_ASYNC_CALL_H__

#include <memory>
#include <ffi.h>
#include <nan.h>
#include <uv.h>

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
    v8::Global<v8::Object> jsCif;
    v8::Global<v8::Function> jsCallback;
    v8::Global<v8::Object> jsResult;
    v8::Global<v8::Object> jsArgv; 

    /**
     * keep javascript cif object to prevent from reclaim by gc.
     */
    void
    SetCif(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& cif);

    void
    SetCallback(
        v8::Isolate* isolate,
        v8::Local<v8::Function>& callback);

    /**
     * keep javascript result object to prevent from reclaim by gc.
     */
    void
    SetResult(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& result);

    /**
     * keep javascript argument variables object to prevent from reclaim by gc.
     */
    void
    SetArgv(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& argv);

 
    /**
     * asynchronous function entry point for javascript
     */
    static NAN_METHOD(FFICallAsync);
    /**
     * worker callback
     */
    static void Run(uv_work_t *req);
    /**
     * worker callback
     */
    static void AsyncFFICall(uv_work_t *req);
    /**
     * clean up function
     */
    static void FinishAsyncFFICall(uv_work_t *req);

    /**
     * You get true the codeBuffer is javascript code
     * This method will help you to determine create asynchronous worker.
     * javasript code have to run with javascript environment. The environment
     * always attach the thread which creates codeBuffer object.
     */
    static bool
    IsJsCodeFunction(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& codeBuffer);
    /**
     * run function in worker thread which is not attached 
     * javasript environment.
     */
    static void 
    RunWorker(
        const Nan::FunctionCallbackInfo<v8::Value>& info);

    /**
     * run function in javascript environment
     */
    static void 
    RunJs(
        const Nan::FunctionCallbackInfo<v8::Value>& info);
    /**
     * called when worker finished
     */
    static void
    FinishedRunning(
        uv_work_t *req,
        int status);
};

}
#endif
#endif
