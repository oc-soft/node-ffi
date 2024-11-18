#ifndef __NODE_FFI_ASYNC_CALL_H__
#define __NODE_FFI_ASYNC_CALL_H__

#include <memory>
#include <list>
#include <ffi.h>
#include <nan.h>
#include <uv.h>

#if __OBJC__ || __OBJC2__
#include <objc/objc.h>
#endif


#ifdef __cplusplus

class callback_info;

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
     * keep javascript callback object to prevent from reclaiming by gc.
     */
     v8::Global<v8::Object> jsCif;

    /**
     * keep javascript callback object to prevent from reclaiming by gc.
     */
    v8::Global<v8::Function> jsCallback;
    /**
     * keep javascript result object to prevent from reclaiming by gc.
     */
    v8::Global<v8::Object> jsResult;
    /**
     * keep javascript argument variables object to prevent from reclaiming
     * by gc.
     */
    v8::Global<v8::Object> jsArgv; 
    /**
     * keep javascript asynchronous codes to prevent from reclaiming by gc.
     */
 
    v8::Global<v8::Object> jsAsyncCodes;
    std::list<callback_info*> asyncCallInfo;
    
    /**
     * keep javascript cif object to prevent from reclaim by gc.
     */
    void
    SetCif(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& cif);

    /**
     * keep javascript callback object to prevent from reclaiming by gc.
     */
    void
    SetCallback(
        v8::Isolate* isolate,
        v8::Local<v8::Function>& callback);

    /**
     * keep javascript result object to prevent from reclaiming by gc.
     */
    void
    SetResult(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& result);

    /**
     * keep javascript argument variables object to prevent from reclaiming
     * by gc.
     */
    void
    SetArgv(
        v8::Isolate* isolate,
        v8::Local<v8::Object>& argv);


    /**
     * keep javascript asynchronous codes to prevent from reclaiming by gc.
     */
    void
    SetAsyncCodes(
        v8::Isolate* isolate,
        v8::Local<v8::Array>& codes);
 
    /**
     * asynchronous function entry point for javascript
     */
    static NAN_METHOD(Run);
    /**
     * worker callback
     */
    static void Run(uv_work_t *req);

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
    /**
     * relate javascript code with current worker
     */
    bool
    RelateAsyncHandle(
        v8::Isolate* isolate,
        v8::Local<v8::Array>& codeBufferArray);

    /**
     * add js code to be called asynchronously.
     */
    void 
    AddAsyncInfo(
        callback_info* info);

    /**
     * get list of js code to be called asynchronously.
     */
    std::list<callback_info*>&
    GetAsyncInfo();

    /**
     * close all async handle which attached into js code.
     */
    void
    CloseAsyncInfo();
};

}
#endif
#endif
