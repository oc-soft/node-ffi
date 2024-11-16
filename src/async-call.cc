#include "node-ffi/async-call.h"

#include <uv.h>
#include "node-ffi/wrap-pointer.h"

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
 * Asynchronous JS wrapper around `ffi_call()`.
 *
 * args[0] - Buffer - the `ffi_cif *`
 * args[1] - Buffer - the C function pointer to invoke
 * args[2] - Buffer - the `void *` buffer big enough to hold the return value
 * args[3] - Buffer - the `void **` array of pointers containing the arguments
 * args[4] - Function - the callback function to invoke when complete
 */

NAN_METHOD(AsyncCall::FFICallAsync) {
  if (info.Length() != 5) {
    Nan::ThrowError("ffi_call_async() requires 5 arguments!");
    return;
  }

  AsyncCall *p = new (std::nothrow) AsyncCall();
  uv_work_t *req = new (std::nothrow) uv_work_t;

  std::unique_ptr<Nan::Callback> callback;
  callback = std::unique_ptr<Nan::Callback>(
    new (std::nothrow) Nan::Callback(v8::Local<v8::Function>::Cast(info[4])));

  if (p && req && callback) {
    // store a persistent references to all the Buffers and the callback function
    v8::Isolate *isolate = info.GetIsolate();
    p->SetCif(reinterpret_cast<ffi_cif*>(node_ffi::UnwrapPointer(isolate, info[0])));
    p->SetFn(FFI_FN(node_ffi::UnwrapPointer(isolate, info[1])));
    p->SetRes(reinterpret_cast<void*>(node_ffi::UnwrapPointer(isolate, info[2])));
    p->SetArgv(reinterpret_cast<void**>(node_ffi::UnwrapPointer(isolate, info[3])));

    p->SetCallback(callback);

    req->data = p;

    uv_queue_work(uv_default_loop(), req,
      AsyncFFICall,
      (uv_after_work_cb)FinishAsyncFFICall);

    info.GetReturnValue().SetUndefined();
  } else {
    info.GetReturnValue().SetUndefined();    
    if (p) {
      delete p;
    }
    if (req) {
      delete req;
    }
    Nan::ThrowError("out of memory");
  }
}

/*
 * Called on the thread pool.
 */
void AsyncCall::AsyncFFICall(uv_work_t *req) {
  AsyncCall *p = reinterpret_cast<AsyncCall *>(req->data);

#if __OBJC__ || __OBJC2__
  @try {
#endif
    ffi_call(
      p->GetCif(),
      p->GetFn(),
      p->GetRes(),
      p->GetArgv()
    );
#if __OBJC__ || __OBJC2__
  } @catch (id ex) {
    p->SetErr(ex);
  }
#endif
}

/*
 * Called after the AsyncFFICall function completes on the thread pool.
 * This gets run on the main loop thread.
 */

void AsyncCall::FinishAsyncFFICall(uv_work_t *req) {
  Nan::HandleScope scope;

  AsyncCall *p = reinterpret_cast<AsyncCall *>(req->data);

  v8::Local<v8::Value> argv[] = { Nan::Null() };
#if __OBJC__ || __OBJC2__
  if (p->HasErr()) {
    v8::Isolate* isolate = Isolate::GetCurrent();
    // an Objective-C error was thrown
    argv[0] = p->GetErr(isolate).LocalChecked();
  }
#endif
  Nan::TryCatch try_catch;

  // invoke the registered callback function
  p->GetCallback()->Call(1, argv);

  // dispose of our persistent handle to the callback function

  // free up our memory (allocated in FFICallAsync)
  delete p;
  delete req;

  if (try_catch.HasCaught()) {
#if NODE_VERSION_AT_LEAST(0, 12, 0)
    Nan::FatalException(try_catch);
#else
    FatalException(try_catch);
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
            FFICallAsync)->GetFunction(ctx).ToLocalChecked());

}
}
// vi: se ts=4 sw=4 et:
