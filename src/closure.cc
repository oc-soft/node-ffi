#include "node-ffi/closure.h"
#include <cstddef>
#include <nan.h>
#include <utility>
#include "node-ffi/async-handle.h"

namespace node_ffi {

/**
 * constructor
 */
Closure::Closure()
    : code(nullptr),
        argc(0),
        resultSize(0)
{
}

/**
 * destructor
 */
Closure::~Closure()
{

    jsFunction.Reset();
    jsErrorFunction.Reset();
}

/**
 * set argc
 */
void
Closure::SetArgc(
    int argc)
{
    this->argc = argc;
}

/**
 * get argc
 */
int
Closure::GetArgc() const
{
    return argc;
}

/**
 * set result size
 */
void
Closure::SetResultSize(
    size_t resultSize)
{
    this->resultSize = resultSize;
}

/**
 * get result size
 */
size_t
Closure::GetResultSize() const
{
    return resultSize;
}
/**
 * set js callback function
 */
void
Closure::SetFunction(
    v8::Isolate* isolate,
    v8::Local<v8::Function>& function)
{
    jsFunction.Reset(isolate, function);
}

/**
 * get js callback function
 */
v8::Local<v8::Function>
Closure::GetFunction(
    v8::Isolate* isolate)
{
    return jsFunction.Get(isolate);
}


/**
 * set js callback function for reporting catched exceptions
 */
void
Closure::SetErrorFunction(
    v8::Isolate* isolate,
    v8::Local<v8::Function>& errorFunction)
{
    jsErrorFunction.Reset(isolate, errorFunction);
}

/**
 * get js callback function for reporting catched exceptions
 */
v8::Local<v8::Function>
Closure::GetErrorFunction(
    v8::Isolate* isolate)
{
    return jsErrorFunction.Get(isolate);
}



/**
 * get the code to call function
 */
void*
Closure::GetCode() const
{
    return code;
}

/**
 * set the code to call function
 */
void
Closure::SetCode(
    void* code)
{
    this->code = code;
}

/**
 * get async handle
 */
std::unique_ptr<node_ffi::AsyncHandle>&
Closure::GetAsyncHandle()
{
    return asyncHandle;
}


/**
 * set async handle
 */
void 
Closure::SetAsyncHandle(
    std::unique_ptr<node_ffi::AsyncHandle>& asyncHandle)
{
    this->asyncHandle = std::move(asyncHandle);
}


/**
 * call error function with specified string
 */
void
Closure::Error(
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
Closure::Error(
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
 * free Closure
 */
void
Closure::Free(
    Closure* info)
{
    if (info)
    {
        // dispose of the Persistent function reference
        info->~Closure();

        // now we can free the closure data
        ffi_closure_free(info);
    }
}

}
