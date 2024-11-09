#include <limits.h>
#include <errno.h>
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS true
#endif
#include <stdint.h>
#include <queue>

#ifdef WIN32
#include "win32-dlfcn.h"
#else
#include <dlfcn.h>
#endif

/* define FFI_BUILDING before including ffi.h to workaround a libffi bug on Windows */
#define FFI_BUILDING
#include <ffi.h>

#include <uv.h>
#include <node_object_wrap.h>
#include <node.h>

#include <nan.h>

#if __OBJC__ || __OBJC2__
  #include <objc/objc.h>
#endif

#define THROW_ERROR_EXCEPTION(x) Nan::ThrowError(x)
#define THROW_ERROR_EXCEPTION_WITH_STATUS_CODE(x, y) Nan::ThrowError(x)

#define FFI_ASYNC_ERROR (ffi_status)1

using namespace v8;
using namespace node;

class callback_info;

/*
 * Converts an arbitrary pointer to a node Buffer with 0-length
 */

void wrap_pointer_cb(char *data, void *hint);

inline Local<Value> WrapPointer(Isolate* isolate,
  char *ptr, size_t length, bool external) {

  Local<Value> res;
  if (!external) {
    Nan::EscapableHandleScope scope;
    res = scope.Escape(Nan::NewBuffer(ptr, length, wrap_pointer_cb, NULL).ToLocalChecked());
  } else {
    Local<ArrayBuffer> ab = ArrayBuffer::New(isolate, sizeof(void*));
    char** destPtr = reinterpret_cast<char **>(ab->Data());
    *destPtr = ptr;
    MaybeLocal<Uint8Array> bfPtr = Buffer::New(isolate, ab, 0, sizeof(void*));

    if (!bfPtr.IsEmpty()) {
      Local<Uint8Array> resArray = bfPtr.ToLocalChecked();
      Maybe<bool> propRes = resArray->DefineOwnProperty(
        isolate->GetCurrentContext(),
        String::NewFromUtf8(isolate, "external").ToLocalChecked(),
        Boolean::New(isolate, true),
        static_cast<PropertyAttribute>(
            PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete));
      if (propRes.FromJust()) {
        res = resArray;
      }
    } else {
      res = bfPtr.ToLocalChecked();
    }
  }
  return res;
}


inline Local<Value> WrapPointer(
  Isolate* isolate, char *ptr, bool external) {
  return WrapPointer(isolate, ptr, 0, external);
}

inline char* UnwrapPointer(const Local<Value>& dataobj, bool external) {
  char* result = nullptr;
  if (dataobj->IsUint8Array()) {
    Local<Uint8Array> ptrArray = Local<Uint8Array>::Cast(dataobj);
    char *ptr = reinterpret_cast<char*>(ptrArray->Buffer()->Data()); 
    ptr += ptrArray->ByteOffset();
    if (!external) {
      result = ptr;
    } else {
      char** ptrRef = reinterpret_cast<char**>(ptr);
      result = *ptrRef;
    }
  }
  return result;
}


inline bool IsExternalPtr(Isolate* isolate, const Local<Value>& dataobj) {
  bool result = false;
  if (dataobj->IsObject()) {
    Local<Object> ptrObj = dataobj.As<Object>(); 
    MaybeLocal<Value> externalValue = ptrObj->Get(isolate->GetCurrentContext(), 
      String::NewFromUtf8(isolate, "external").ToLocalChecked());
    if (!externalValue.IsEmpty()) {
      result = externalValue.ToLocalChecked()->IsTrue();
    }
  } 
  return result;
}

inline char* UnwrapPointer(Isolate* isolate, const Local<Value>& dataobj) {
  return UnwrapPointer(dataobj, IsExternalPtr(isolate, dataobj));
}
 
/*
 * Class used to store stuff during async ffi_call() invokations.
 */

class AsyncCallParams {
  public:
    AsyncCallParams() = default;

    ~AsyncCallParams();
#if __OBJC__ || __OBJC2__
    bool
    HasErr() const;

    id
    GetErrRef() const;

    void
    SetErr(
        id err);

    MaybeLocal<v8::Object>
    GetErr(Isolate* isolate);
#endif

    void
    SetCif(
        ffi_cif* cif);

    ffi_cif*
    GetCif() const;

    void
    (*GetFn() const)(void);

    void
    SetFn(
        void (*fn)(void));

    void*
    GetRes() const;

    void
    SetRes(
        void* res);

    void**
    GetArgv() const;

    void
    SetArgv(
        void** argv);

    void
    SetCallback(
        std::unique_ptr<Nan::Callback>& callback);

    std::unique_ptr<Nan::Callback>&
    GetCallback();

  private:
#if __OBJC__ || __OBJC2__
    id err = Nil;
#endif
    ffi_cif *cif = nullptr;
    void (*fn)(void) = nullptr;
    void *res = nullptr;
    void **argv = nullptr;
    std::unique_ptr<Nan::Callback> callback;
};

class FFI {
  public:
    static NAN_MODULE_INIT(InitializeStaticFunctions);
    static NAN_MODULE_INIT(InitializeBindings);

  protected:
    static NAN_METHOD(FFIPrepCif);
    static NAN_METHOD(FFIPrepCifVar);
    static NAN_METHOD(FFICall);
    static NAN_METHOD(FFICallAsync);
    static void AsyncFFICall(uv_work_t *req);
    static void FinishAsyncFFICall(uv_work_t *req);

    static NAN_METHOD(Strtoul);
};



class ThreadedCallbackInvokation;

class CallbackInfo {
  public:
    static NAN_MODULE_INIT(Initialize);
    static void WatcherCallback(uv_async_t *w, int revents);

  protected:
    static void DispatchToV8(callback_info *self, void *retval, void **parameters, bool dispatched = false);
    static void Invoke(ffi_cif *cif, void *retval, void **parameters, void *user_data);
    static NAN_METHOD(Callback);

  private:
#ifdef WIN32
    static DWORD g_threadID;
#else
    static uv_thread_t          g_mainthread;
#endif // WIN32
    static uv_mutex_t    g_queue_mutex;
    static std::queue<ThreadedCallbackInvokation *> g_queue;
    static uv_async_t         g_async;
};

/**
 *   Synchronization object to ensure following order of execution:
 *   -> WaitForExecution()     invoked
 *   -> SignalDoneExecuting()  returned
 *   -> WaitForExecution()     returned
 *
 *   ^WaitForExecution() must always be called from the thread which owns the object
 */

class ThreadedCallbackInvokation {
  public:
    ThreadedCallbackInvokation(callback_info *cbinfo, void *retval, void **parameters);
    ~ThreadedCallbackInvokation();

    void SignalDoneExecuting();
    void WaitForExecution();

    void *m_retval;
    void **m_parameters;
    callback_info *m_cbinfo;

  private:
    uv_cond_t m_cond;
    uv_mutex_t m_mutex;
};
