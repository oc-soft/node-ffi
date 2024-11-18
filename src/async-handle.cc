#include "node-ffi/async-handle.h"
#include <nan.h>
#include "callback-info-i.h"
#include "node-ffi/callback.h"

namespace node_ffi {

/**
 * constructor
 */
AsyncHandle::AsyncHandle(
    callback_info* callbackInfo)
    :   info(callbackInfo)
{
    uv_async_init(Nan::GetCurrentEventLoop(), this, RunInEventLoop);
}

/**
 * destructor
 */
AsyncHandle::~AsyncHandle()
{
    uv_async_t* handle = this;
    
    uv_close(reinterpret_cast<uv_handle_t*>(handle), nullptr);
}



/**
 * allocate argv
 */
bool
AsyncHandle::AllocateArgv(
    int argc)
{
    bool result = false;
    std::unique_ptr<void* []> argv(new (std::nothrow) void*[argc]);
    if (argv) {
        result = true;
    }
    if (result) {
        argvRef = std::move(argv);
    }
    return result; 
}

/**
 * allocate result buffer
 */
bool
AsyncHandle::AllocateResult(
    size_t resultSize)
{
    bool result = false;
    std::unique_ptr<char []> resultBuf(new (std::nothrow) char[resultSize]);
    if (resultBuf) {
        result = true;
    }
    if (result) {
        resultRef = std::move(resultBuf);
    }
    return result; 
}

/**
 * create async handle
 */
AsyncHandle*
AsyncHandle::NewAsyncHandle(
    callback_info* info)
{
    int state;
    state = 0;
    AsyncHandle* result = new (std::nothrow) AsyncHandle(info);

    state = result ? 0 : -1;

    if (state == 0) {
        state = result->AllocateArgv(info->GetArgc()) ? 0 : -1;

    }
    if (state == 0) {
        state = result->AllocateResult(info->GetResultSize()) ? 0 : -1;
    }
    if (state) {
        delete result;
        result = nullptr;
    }    
    return result;
}

/**
 * run in uv event loop
 */
void
AsyncHandle::RunInEventLoop(
    uv_async_t* asyncHandle)
{
    AsyncHandle* self = reinterpret_cast<AsyncHandle*>(asyncHandle);
    Callback::DispatchToV8(
        self->info, 
        reinterpret_cast<void*>(self->resultRef.get()),
        self->argvRef.get(), true);
}


}
