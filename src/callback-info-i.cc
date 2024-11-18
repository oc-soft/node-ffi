#include "callback-info-i.h"
#include <cstddef>
#include <nan.h>
#include <utility>
#include "node-ffi/async-handle.h"


/**
 * constructor
 */
callback_info::callback_info()
    : code(nullptr),
        argc(0),
        resultSize(0)
{
}

/**
 * destructor
 */
callback_info::~callback_info()
{

    jsFunction.Reset();
    jsErrorFunction.Reset();
}

/**
 * set argc
 */
void
callback_info::SetArgc(
    int argc)
{
    this->argc = argc;
}

/**
 * get argc
 */
int
callback_info::GetArgc() const
{
    return argc;
}

/**
 * set result size
 */
void
callback_info::SetResultSize(
    size_t resultSize)
{
    this->resultSize = resultSize;
}

/**
 * get result size
 */
size_t
callback_info::GetResultSize() const
{
    return resultSize;
}
/**
 * set js callback function
 */
void
callback_info::SetFunction(
    v8::Isolate* isolate,
    v8::Local<v8::Function>& function)
{
    jsFunction.Reset(isolate, function);
}

/**
 * get js callback function
 */
v8::Local<v8::Function>
callback_info::GetFunction(
    v8::Isolate* isolate)
{
    return jsFunction.Get(isolate);
}


/**
 * set js callback function for reporting catched exceptions
 */
void
callback_info::SetErrorFunction(
    v8::Isolate* isolate,
    v8::Local<v8::Function>& errorFunction)
{
    jsErrorFunction.Reset(isolate, errorFunction);
}

/**
 * get js callback function for reporting catched exceptions
 */
v8::Local<v8::Function>
callback_info::GetErrorFunction(
    v8::Isolate* isolate)
{
    return jsErrorFunction.Get(isolate);
}



/**
 * get the code to call function
 */
void*
callback_info::GetCode() const
{
    return code;
}

/**
 * set the code to call function
 */
void
callback_info::SetCode(
    void* code)
{
    this->code = code;
}

/**
 * get async handle
 */
std::unique_ptr<node_ffi::AsyncHandle>&
callback_info::GetAsyncHandle()
{
    return asyncHandle;
}


/**
 * set async handle
 */
void 
callback_info::SetAsyncHandle(
    std::unique_ptr<node_ffi::AsyncHandle>& asyncHandle)
{
    this->asyncHandle = std::move(asyncHandle);
}


/**
 * call error function with specified string
 */
void
callback_info::Error(
    v8::Isolate* isolate,
    const char* errorString)
{
    Nan::HandleScope scope;
    v8::Local<v8::Value> errorValue;
    errorValue = Nan::New<v8::String>(errorString).ToLocalChecked();
    Error(isolate, errorValue);
}


/**
 * call error function with specified string
 */
void
callback_info::Error(
    v8::Isolate* isolate,
    v8::Local<v8::Value> errorValue)
{
    v8::Local<v8::Function> jsErrorFunction;
    jsErrorFunction = GetErrorFunction(isolate);
    if (!jsErrorFunction.IsEmpty()) {
        v8::Local<v8::Value> errorFunctionArgv[1];
        errorFunctionArgv[0] = errorValue;
        Nan::Callback nativeCallErrorJs = Nan::Callback(
            jsErrorFunction);
        nativeCallErrorJs.Call(1, errorFunctionArgv);
    }
}

/**
 * free callback_info
 */
void
callback_info::Free(
    callback_info* info)
{
    if (info)
    {
        // dispose of the Persistent function reference
        info->~callback_info();

        // now we can free the closure data
        ffi_closure_free(info);
    }
}


