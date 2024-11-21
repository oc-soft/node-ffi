#include "node-ffi/async-call.h"
#include <uv.h>
#include <algorithm>
#include "node-ffi/callback.h"
#include "node-ffi/wrap-pointer.h"
#include "node-ffi/async-handle.h"
#include "node-ffi/closure.h"

#if __OBJC__ || __OBJC2__
#include "objc-object-wrap.h"
#endif

namespace node_ffi {

AsyncCall::~AsyncCall()
{
#if __OBJC__ || __OBJC2__
    SetErr(Nil);
#endif
}

#if __OBJC__ || __OBJC2__

bool
AsyncCall::HasErr() const
{
    return GetErrRef() != Nil;
}

id
AsyncCall::GetErrRef() const
{
    return err;
}

void
AsyncCall::SetErr(
    id err)
{
    if (err) {
        [err retain];
    }
    if (this->err) {
        [this->err release];
    }
    this->err = err;
}

MaybeLocal<v8::Object>
AsyncCall::GetErr(
    Isolate* isolate)
{
    MaybeLocal<v8::Object> result;
    if (err) {
        result = ObjcObjectWrap::New(isolate, err); 
    }
    return result;
}
#endif

void
AsyncCall::SetCif(
    ffi_cif* cif)
{
    this->cif = cif;
}

ffi_cif*
AsyncCall::GetCif() const
{
    return cif;
}

void
(*AsyncCall::GetFn() const)(void)
{
    return fn;
}

void
AsyncCall::SetFn(
    void (*fn)(void))
{
    this->fn = fn;
}

void*
AsyncCall::GetRes() const
{
    return res;
}

void
AsyncCall::SetRes(
    void* res)
{
    this->res = res;
}

void**
AsyncCall::GetArgv() const
{
    return argv;
}

void
AsyncCall::SetArgv(
    void** argv)
{
    this->argv = argv;
}

void
AsyncCall::SetCallback(
    std::unique_ptr<Nan::Callback>& callback)
{
    std::unique_ptr<Nan::Callback> tmpValue;
    std::swap(tmpValue, callback);
    std::swap(tmpValue, this->callback);
}

std::unique_ptr<Nan::Callback>&
AsyncCall::GetCallback() 
{
    return callback;
}

/**
 * keep javascript cif object to prevent from reclaim by gc.
 */
void
AsyncCall::SetCif(
    v8::Isolate* isolate,
    v8::Local<v8::Object>& cif)
{
    jsCif.Reset(isolate, cif);
}

void
AsyncCall::SetCallback(
    v8::Isolate* isolate,
    v8::Local<v8::Function>& callback)
{
    jsCallback.Reset(isolate, callback);
}

/**
 * keep javascript result object to prevent from reclaim by gc.
 */
void
AsyncCall::SetResult(
    v8::Isolate* isolate,
    v8::Local<v8::Object>& result)
{
    jsResult.Reset(isolate, result);
}

/**
 * keep javascript argument variables object to prevent from reclaim by gc.
 */
void
AsyncCall::SetArgv(
    v8::Isolate* isolate,
    v8::Local<v8::Object>& argv)
{
    jsArgv.Reset(isolate, argv); 
}

/**
 * keep javascript asynchronous codes to prevent from reclaiming by gc.
 */
void
AsyncCall::SetAsyncCodes(
    v8::Isolate* isolate,
    v8::Local<v8::Array>& codes)
{
    jsArgv.Reset(isolate, codes); 
}

/**
 * relate javascript code with current worker
 */
bool
AsyncCall::RelateAsyncHandle(
    v8::Isolate* isolate,
    v8::Local<v8::Array>& codeBufferArray)
{
    struct IterInfo {
        v8::Isolate* isolate;
        AsyncCall& self;
        bool result; 
        IterInfo(
            v8::Isolate* isolateParam, 
            AsyncCall& asyncCall)
            :   isolate(isolateParam),
                self(asyncCall),
                result(true) {
        }
    };
    IterInfo info(isolate, *this);
    codeBufferArray->Iterate(
        isolate->GetCurrentContext(),
        [](uint32_t idx,
            v8::Local<v8::Value> element,
            void* user_data) -> v8::Array::CallbackResult {
        Nan::HandleScope scope;
        IterInfo* info = reinterpret_cast<IterInfo*>(user_data);
        v8::Array::CallbackResult result;
        result = v8::Array::CallbackResult::kContinue; 
        if (element->IsObject()) {
            v8::Local<v8::Object> elemObj;
            elemObj = v8::Local<v8::Object>::Cast(element);
            Closure* cbInfo = nullptr;
            bool await = false;
            if (DecodeAsyncCallback(info->isolate, elemObj, &cbInfo, await)) {
                std::unique_ptr<AsyncHandle> asyncHandle(
                    node_ffi::AsyncHandle::NewAsyncHandle(cbInfo, await));
                if (asyncHandle) {
                    cbInfo->SetAsyncHandle(asyncHandle);
                    info->self.AddAsyncInfo(cbInfo);
                } else {
                    info->result = false;
                    result = v8::Array::CallbackResult::kBreak; 
                }
            }
        }
        return result;
    }, &info); 
    return info.result;
}

/**
 * decode callback info and parameters for async handle
 * @param isolate 
 * @param codeAsyncContainer contains code object and parameter for
 * async handle
 * @param info output parameter
 * @param await flag output parameter
 * @return you get true if 
 */
bool 
AsyncCall::DecodeAsyncCallback(
    v8::Isolate* isolate,
    v8::Local<v8::Object>& codeAsyncContainer,
    Closure** info,
    bool& await)
{
    bool result;
    result = false;
    Closure* cbInfo;
    cbInfo = nullptr;
    cbInfo = Callback::DecodeCallbackInfo(isolate, codeAsyncContainer);
    await = false;
    if (cbInfo) {
        result = true;
    } 
    if (!result) {
        v8::MaybeLocal<v8::Value> codeValue;
        codeValue = codeAsyncContainer->Get(
            isolate->GetCurrentContext(),
            v8::String::NewFromUtf8(isolate, "code").ToLocalChecked()); 
        if (!codeValue.IsEmpty()) {
            v8::Local<v8::Value> codeValue0 = codeValue.ToLocalChecked();
            if (codeValue0->IsObject()) {
                v8::Local<v8::Object> codeObj;
                codeObj = v8::Local<v8::Object>::Cast(codeValue0);
                cbInfo = Callback::DecodeCallbackInfo(isolate, codeObj);
                result = cbInfo ? true : false;
            }
        }
        if (result) {
            v8::MaybeLocal<v8::Value> awaitValue;
            awaitValue = codeAsyncContainer->Get(
                isolate->GetCurrentContext(),
                v8::String::NewFromUtf8(isolate, "await").ToLocalChecked()); 
            if (!awaitValue.IsEmpty()) {
                await = awaitValue.ToLocalChecked()->IsTrue();
            }
        } 
    }
    if (result) {
        *info = cbInfo;
    }
    return result;
}

/**
 * add js code to be called asynchronously.
 */
void
AsyncCall::AddAsyncInfo(
    Closure* info)
{
    asyncCallInfo.push_back(info); 
}

/**
 * get list of js code to be called asynchronously.
 */
std::list<Closure*>&
AsyncCall::GetAsyncInfo()
{
    return asyncCallInfo;
}

/**
 * close all async handle which attached into js code.
 */
void
AsyncCall::CloseAsyncInfo()
{
    struct ClearInfo {
        void operator()(Closure* info)
        {
            info->GetAsyncHandle().reset();
        }
    }; 
    std::for_each(GetAsyncInfo().begin(), GetAsyncInfo().end(), ClearInfo());
    GetAsyncInfo().clear();
}

/**
 * run specified method in worker thread
 */
void
AsyncCall::RunWorker(
    const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    int status;
    AsyncCall* p = nullptr;
    status = 0;
    p = new (std::nothrow) AsyncCall();
    if (!p) {
        Nan::ThrowError("can not allocate async native instance");
        status = -1;
    }
    uv_work_t* req = nullptr;
    if (status == 0) {
        req = new (std::nothrow) uv_work_t;
        if (!req) {
            Nan::ThrowError("can not allocate worker instance");
            status = -1;
        }
    }
    v8::Local<v8::Object> jsResult;
    if (status == 0) {
        status = info[2]->IsObject() ? 0 : -1;
        if (status) {
            Nan::ThrowError("The 3rd argument must be object");
        } else {
            jsResult = v8::Local<v8::Object>::Cast(info[2]);
        }
    }
    v8::Local<v8::Object> jsArgv;
    if (status == 0) {
        status = info[3]->IsObject() ? 0 : -1;
        if (status) {
            Nan::ThrowError("The 4th argument must be object");
        } else {
            jsArgv = v8::Local<v8::Object>::Cast(info[3]);
        }
    }
    v8::Local<v8::Function> jsFunc;
    if (status == 0) {
        status = info[4]->IsFunction() ? 0 : -1;
        if (status) {
            Nan::ThrowError("The 4th argument must be function");
        } else { 
            jsFunc = v8::Local<v8::Function>::Cast(info[4]);
        }
    }
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Array> jsAsyncCodes;
    if (status == 0) {
        if (info.Length() > 4 && info[5]->IsArray()) {
            jsAsyncCodes = v8::Local<v8::Array>::Cast(info[5]); 
            status = p->RelateAsyncHandle(isolate, jsAsyncCodes) ? 0 : -1;
            if (status) {
                p->CloseAsyncInfo();
            }
        }
    }
    if (status == 0) {
        // store a persistent references to all the Buffers and the callback function
        p->SetCif(reinterpret_cast<ffi_cif*>(UnwrapPointer(isolate, info[0])));
        p->SetFn(FFI_FN(UnwrapPointer(isolate, info[1])));
        p->SetRes(reinterpret_cast<void*>(UnwrapPointer(isolate, info[2])));
        p->SetArgv(reinterpret_cast<void**>(UnwrapPointer(isolate, info[3])));
        p->SetResult(isolate, jsResult);
        p->SetArgv(isolate, jsArgv);
        p->SetCallback(isolate, jsFunc);
        req->data = p;
        uv_queue_work(Nan::GetCurrentEventLoop(), req, Run, FinishedRunning);
        req = nullptr;
        p = nullptr;
    }
    info.GetReturnValue().SetUndefined();    
    if (p) {
        delete p;
    }
    if (req) {
        delete req;
    }
}

/**
 * run callback in javascript environment
 */
void
AsyncCall::RunJs(
    const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    int status = 0;
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Object> funcObj;
    funcObj = info[1]->ToObject(
        isolate->GetCurrentContext()).ToLocalChecked();
     
    Closure* callbackInfo = nullptr;
    callbackInfo = Callback::DecodeCallbackInfo(info.GetIsolate(), funcObj);
    status = callbackInfo ? 0 : -1; 

    if (status == 0) {
        Nan::HandleScope scope;

        Callback::DispatchToV8(callbackInfo,
            reinterpret_cast<void*>(UnwrapPointer(isolate, info[2])),
            reinterpret_cast<void**>(UnwrapPointer(isolate, info[3])),
            true);

    }
    if (status == 0) {
        Nan::HandleScope scope;
        v8::Local<v8::Value> argv[] = { Nan::Null() };
        Nan::AsyncResource asyncRes("node-ffi-async");

        Nan::Callback callbackNative(v8::Local<v8::Function>::Cast(info[4]));
        callbackNative.Call(1, argv, &asyncRes);
    }
}
/**
 * You get true the codeBuffer is javascript code
 * This method will help you to determine create asynchronous worker.
 * javasript code have to run with javascript environment. The environment
 * always attach the thread which creates codeBuffer object.
 */
bool
AsyncCall::IsJsCodeFunction(
    v8::Isolate* isolate,
    v8::Local<v8::Object>& codeBuffer)
{    
    bool result = false;
    result = Callback::DecodeCallbackInfo(isolate, codeBuffer) != nullptr;
    return result;
}


/**
 * Asynchronous JS wrapper around `ffi_call()`.
 *
 * args[0] - Buffer - the `ffi_cif *`
 * args[1] - Buffer - the C function pointer to invoke
 * args[2] - Buffer - the `void *` buffer big enough to hold the return value
 * args[3] - Buffer - the `void **` array of pointers containing the arguments
 * args[4] - Function - the callback function to invoke when complete
 * args[5] - array - code buffer array. each item is javascript callbacks which
 * has to be run on the thread attached javascript enviroment.
 */

NAN_METHOD(AsyncCall::Run) {
    Nan::HandleScope scope;
    int state;
    state = 0;
    if (info.Length() < 5) {
        Nan::ThrowError("ffi_call_async() requires 5 arguments!");
        state = -1; 
    }
    
    v8::Local<v8::Object> codeBuff;
    if (state == 0) {
        if (info[1]->IsObject()) {
            codeBuff = v8::Local<v8::Object>::Cast(info[1]);
        } else {
            Nan::ThrowError("2nd argumentt must be object.");
            state = -1;
        }
    }
    if (state == 0) {
        if (!IsJsCodeFunction(info.GetIsolate(), codeBuff)) {
            RunWorker(info);
        } else {
            RunJs(info);
        }
    }
}

/**
 * Called on the thread pool.
 */
void
AsyncCall::Run(
    uv_work_t *req) {
    AsyncCall *p = reinterpret_cast<AsyncCall *>(req->data);
#if __OBJC__ || __OBJC2__
    @try {
#endif
    ffi_call(
        p->GetCif(),
        p->GetFn(),
        p->GetRes(),
        p->GetArgv());
#if __OBJC__ || __OBJC2__
    } @catch (id ex) {
        p->SetErr(ex);
    }
#endif
}

/**
 * finished async run method
 */
void
AsyncCall::FinishedRunning(
    uv_work_t *req,
    int status)
{
    Nan::HandleScope scope;
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    AsyncCall *p = reinterpret_cast<AsyncCall*>(req->data);

    v8::Local<v8::Value> argv[] = { Nan::Null() };
#if __OBJC__ || __OBJC2__
    if (p->HasErr()) {

        // an Objective-C error was thrown
        argv[0] = p->GetErr(isolate).LocalChecked();
    }
#endif
    Nan::TryCatch try_catch;

    // invoke the registered callback function
    
    {
        v8::Local<v8::Function> func = p->jsCallback.Get(isolate);
        if (!func.IsEmpty()) {
            Nan::AsyncResource asyncRes("node-ffi-async");
            Nan::Callback callbackNative(func);
            callbackNative.Call(1, argv, &asyncRes);
        }
        p->jsCif.Reset();
        p->jsCallback.Reset();
        p->jsResult.Reset();
        p->jsArgv.Reset(); 
        p->CloseAsyncInfo();
    }
    // dispose of our persistent handle to the callback function

    // free up our memory (allocated in FFICallAsync)
    delete p;
    delete req;

    if (try_catch.HasCaught()) {
#if NODE_VERSION_AT_LEAST(0, 12, 0)
        Nan::FatalException(try_catch);
#else
        Nan::FatalException(try_catch);
#endif
    }
}



/**
 * register aynchronous functions
 */
NAN_MODULE_INIT(AsyncCall::Register)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
 
    Nan::Set(target,
        Nan::New<v8::String>("ffi_call_async").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(
            Run)->GetFunction(ctx).ToLocalChecked());

}


}
// vi: se ts=4 sw=4 et:
