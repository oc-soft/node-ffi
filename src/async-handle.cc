#include "node-ffi/async-handle.h"
#include <nan.h>
#include "node-ffi/closure.h"
#include "node-ffi/callback.h"

namespace node_ffi {

/**
 * constructor
 */
AsyncHandle::AsyncHandle(
    Closure* callbackInfo)
    :   info(callbackInfo),
        loopThread(uv_thread_self())
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
    if (condition) {
        uv_cond_destroy(condition.get());
    }
    if (conditionMutex) {
        uv_mutex_destroy(conditionMutex.get());
    }
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
 * initialize condition mutex
 */
bool
AsyncHandle::InitAwaitOption()
{
    bool result;
    conditionMutex.reset(new (std::nothrow) uv_mutex_t);
    result = conditionMutex ? true : false;
    if (result) {
        result = uv_mutex_init(conditionMutex.get()) == 0;
        if (!result) {
            conditionMutex.reset();
        }
    }
    if (result) {
        condition.reset(new (std::nothrow) uv_cond_t);
        result = condition ? true : false;
    }
    if (result) {
        result = uv_cond_init(condition.get()) == 0;
        if (!result) {
            condition.reset();
        } 
    }

    if (!result) {
        TearDownAwaitOption();
    }
    return result;
}

/**
 * tear down condition mutex
 */
void
AsyncHandle::TearDownAwaitOption()
{
    if (condition) {
        uv_cond_destroy(condition.get());
        condition.reset();
    }
    if (conditionMutex) {
        uv_mutex_destroy(conditionMutex.get());
        conditionMutex.reset();
    }
}

/**
 * wait condition
 */
void
AsyncHandle::WaitCondition()
{
    if (condition &&  conditionMutex) {
        uv_cond_wait(condition.get(), conditionMutex.get());
    }
}


/**
 * lock condition mutex
 */
void
AsyncHandle::LockConditionMutex()
{
    if (conditionMutex) {
        uv_mutex_lock(conditionMutex.get());
    }
}

/**
 * unlock condition mutex
 */
void
AsyncHandle::UnlockConditionMutex()
{
    if (conditionMutex) {
        uv_mutex_unlock(conditionMutex.get());
    }
}

/**
 * If result is true then worker thread will be wait javascript code.
 */
 bool
AsyncHandle::IsAwait() const
{
    return conditionMutex && condition;
}

/**
 * create async handle
 */
AsyncHandle*
AsyncHandle::NewAsyncHandle(
    Closure* info,
    bool await)
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
    if (state == 0) {
        if (await) {
            state = result->InitAwaitOption() ? 0 : -1;
        }
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
    if (self->IsAwait()) {
        self->LockConditionMutex();
    }
    
    Callback::DispatchToV8(
        self->info, 
        reinterpret_cast<void*>(self->resultRef.get()),
        self->argvRef.get(), true);
    if (self->IsAwait()) {
        uv_cond_signal(self->condition.get());
        self->UnlockConditionMutex();
    }
}

}
