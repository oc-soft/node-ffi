#ifndef __NODE_FFI_ASYNC_HANDLE_H__
#define __NODE_FFI_ASYNC_HANDLE_H__

#include <memory>
#include <uv.h>

#ifdef __cplusplus

class callback_info;

namespace node_ffi {

class AsyncHandle : public uv_async_t {
    friend class callback_info;
    callback_info* info;
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
    AsyncHandle(callback_info* callbackInfo);

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
        callback_info* callbackInfo);
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
