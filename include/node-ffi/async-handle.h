#ifndef __NODE_FFI_ASYNC_HANDLE_H__
#define __NODE_FFI_ASYNC_HANDLE_H__

#include <memory>
#include <uv.h>

#ifdef __cplusplus


namespace node_ffi {

class Closure;

class Callback;

class AsyncHandle : public uv_async_t {
    friend class Closure;
    friend class Callback;
    /**
     * javascript code container
     */
    Closure* info;

    /**
     * eventt loop thread id
     */ 
    uv_thread_t loopThread;

    /**
     * used for condition 
     */
    std::unique_ptr<uv_mutex_t> conditionMutex;

    /**
     * Condition indicates whether resultRef and argRef processed by
     * javascript code.
     */
    std::unique_ptr<uv_cond_t> condition;
    // these two are required for creating proper sized WrapPointer
    // buffer instances

    /**
     * arguments last passed in worker thread.
     */
    std::unique_ptr<void*[]> argvRef;
   
    /**
     * result referece last passed in worker thread
     */
    std::unique_ptr<char[]> resultRef; 

    /**
     * constructor
     */
    AsyncHandle(Closure* callbackInfo);

    /**
     * allocate argv
     */
    bool
    AllocateArgv(
        int argc);

    /**
     * allocate result buffer
     */
    bool
    AllocateResult(
        size_t resultSize);


    /**
     * initialize condition mutex
     */
    bool
    InitAwaitOption();

    /**
     * tear down condition mutex
     */
    void
    TearDownAwaitOption();

    /**
     * wait condition
     */
    void
    WaitCondition();

    /**
     * lock condition mutex
     */
    void
    LockConditionMutex();

    /**
     * unlock condition mutex
     */
    void
    UnlockConditionMutex();

    /**
     * If result is true then worker thread will be wait javascript code.
     */
     bool
    IsAwait() const; 
public:

    /**
     * destructor
     */
    ~AsyncHandle();

    /**
     * create async handle
     */
    static AsyncHandle*
    NewAsyncHandle(
        Closure* callbackInfo,
        bool await = false);
    /**
     * run in uv event loop
     */
    static void
    RunInEventLoop(
        uv_async_t* asyncHandle);

};


}
#endif

// vi: se ts=4 sw=4 et:
#endif
