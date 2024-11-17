#include "callback-info-i.h"
#include <nan.h>


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
 * get js callback function for reporting catched exceptions
 */
const std::unique_ptr<Nan::Callback>&
callback_info::GetErrorFunction() const
{
    return errorFunction;
}

/**
 * set js callback function for reporting catched exceptions
 */
void 
callback_info::SetErrorFunction(
    std::unique_ptr<Nan::Callback>& errorFunction) 
{
    std::unique_ptr<Nan::Callback> tmpValue;
    std::swap(tmpValue, errorFunction);
    std::swap(this->errorFunction, tmpValue);
}

/**
 * get js callback function the closure represents
 */
const std::unique_ptr<Nan::Callback>&
callback_info::GetFunction() const
{
    return function;
}

/**
 * set js callback function the closure represents
 */
void 
callback_info::SetFunction(
    std::unique_ptr<Nan::Callback>& function)
{
    std::unique_ptr<Nan::Callback> tmpValue;
    std::swap(tmpValue, function);
    std::swap(this->function, tmpValue);
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

