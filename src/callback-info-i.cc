#include "callback-info.h"
#include <nan.h>

#include "callback-info-i.h"

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
    return errorFunction;
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

