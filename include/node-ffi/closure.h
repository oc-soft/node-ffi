#ifndef __NODE_FFI_CLOSURE_H__
#define __NODE_FFI_CLOSURE_H__

#include <memory>
#include <ffi.h>
#include <nan.h>
#include <uv.h>

#ifdef __cplusplus

namespace node_ffi {
class AsyncHandle;

/*
 * One of these structs gets created for each `ffi.Callback()` invokation in
 * JavaScript-land. It contains all the necessary information when invoking the
 * pointer to proxy back to JS-land properly. It gets created by
 * `ffi_closure_alloc()`, and free'd in the closure_pointer_cb function.
 */

class Closure : public ffi_closure {
    /**
     * the executable function pointer
     */
    void *code;
    /**
     * JS callback function for reporting catched exceptions for the process'
     * event loop
     */
    v8::Global<v8::Function> jsErrorFunction;

    /**
     * JS callback function the closure represents
     */
    v8::Global<v8::Function> jsFunction;


    // these two are required for creating proper sized WrapPointer
    // buffer instances
    /**
     * the number of arguments this function expects
     */
    int argc;
    /**
     * the size of the result pointer
     */
    size_t resultSize;


    /**
     * async handle
     */
    std::unique_ptr<node_ffi::AsyncHandle> asyncHandle;


public:
    /**
     * constructor
     */
    Closure();

    /**
     * destructor
     */
    ~Closure();

    /**
     * set argc
     */
    void 
    SetArgc(
        int argc);

    /**
     * get argc
     */
    int
    GetArgc() const;

    /**
     * set result size
     */
    void 
    SetResultSize(
        size_t resultSize);

    /**
     * get result size
     */
    size_t
    GetResultSize() const;

    /**
     * set js callback function
     */
    void
    SetFunction(
        v8::Isolate* isolate,
        v8::Local<v8::Function>& function);

    /**
     * get js callback function
     */
    v8::Local<v8::Function>
    GetFunction(
        v8::Isolate* isolate);

    /**
     * set js callback function for reporting catched exceptions
     */
    void
    SetErrorFunction(
        v8::Isolate* isolate,
        v8::Local<v8::Function>& errorFunction);

    /**
     * get js callback function for reporting catched exceptions
     */
    v8::Local<v8::Function>
    GetErrorFunction(
        v8::Isolate* isolate);
 
    /**
     * get the code to call function
     */
    void*
    GetCode() const;

    /**
     * set the code to call function
     */
    void
    SetCode(
        void* code);

    /**
     * get async handle
     */
    std::unique_ptr<node_ffi::AsyncHandle>&
    GetAsyncHandle();


    /**
     * set async handle
     */
    void 
    SetAsyncHandle(
        std::unique_ptr<node_ffi::AsyncHandle>& asyncHandle);


    /**
     * free allocated object by ffi_closure_alloc
     */
    static void
    Free(Closure* info);

    /**
     * call error function with specified string
     */
    void
    Error(
        v8::Isolate* isolate,
        v8::Local<v8::Value> errorObj);
 
    /**
     * call error function with specified string
     */
    void
    Error(
        v8::Isolate* isolate,
        const char* errorString);
    

};

}
#endif
// vi: se ts=4 sw=4 et:
#endif
