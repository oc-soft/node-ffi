#include "node-ffi/callback.h"
// https://raw.githubusercontent.com/ghostoy/node-ffi/master/src/callback_info.cc
// Reference:
//   http://www.bufferoverflow.ch/cgi-bin/dwww/usr/share/doc/libffi5/html/The-Closure-API.html

#include <node.h>
#include <string>
#include "node-ffi.h"
#include "callback-info-i.h"
#include "code-object.h"
#include "threaded_callback_invokation.h"
#include "node-ffi/wrap-pointer.h"

using namespace v8;
using namespace node;

#if !(NODE_VERSION_AT_LEAST(0, 11, 15))
  #ifdef WIN32
    int uv_thread_equal(const uv_thread_t* t1, const uv_thread_t* t2) {
      return *t1 == *t2;
    }
  #else
    #include <pthread.h>
    int uv_thread_equal(const uv_thread_t* t1, const uv_thread_t* t2) {
      return pthread_equal(*t1, *t2);
    }
  #endif
#endif

namespace node_ffi {
#ifdef WIN32
DWORD Callback::g_threadID;
#else
uv_thread_t Callback::g_mainthread;
#endif
uv_mutex_t    Callback::g_queue_mutex;
std::queue<ThreadedCallbackInvokation *> Callback::g_queue;
uv_async_t         Callback::g_async;

/*
 * Invokes the JS callback function.
 */

void Callback::DispatchToV8(callback_info *info, void *retval, void **parameters, bool dispatched) {
  Nan::HandleScope scope;

  static const char* errorMessage = "ffi fatal: callback has been garbage collected!";

  if (!info->GetFunction()) {
    // throw an error instead of segfaulting.
    // see: https://github.com/rbranson/node-ffi/issues/72
    if (dispatched) {
        Local<Value> errorFunctionArgv[1];
        errorFunctionArgv[0] = Nan::New<String>(errorMessage).ToLocalChecked();
        info->GetErrorFunction()->Call(1, errorFunctionArgv);
    }
    else {
      Nan::ThrowError(errorMessage);
    }
  } else {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();

    MaybeLocal<Object> resBuf = Buffer::New(isolate,
      info->GetResultSize());
    MaybeLocal<Object> argBuf = Buffer::New(isolate,
      sizeof(char *) * info->GetArgc());
    
    if (!resBuf.IsEmpty() && !argBuf.IsEmpty()) {
      std::memcpy(Buffer::Data(resBuf.ToLocalChecked()),
        retval, info->GetResultSize());

      std::memcpy(Buffer::Data(argBuf.ToLocalChecked()), parameters,
        sizeof(char*) * info->GetArgc());
    
      // invoke the registered callback function
      Local<Value> functionArgv[2];
      functionArgv[0] = resBuf.ToLocalChecked();
      functionArgv[1] = argBuf.ToLocalChecked();
     
      Local<Value> e = info->GetFunction()->Call(2, functionArgv);

      if (!e->IsUndefined()) {
        if (dispatched) {
          Local<Value> errorFunctionArgv[1];
          errorFunctionArgv[0] = e;
          info->GetErrorFunction()->Call(1, errorFunctionArgv);
        } else {
          Nan::ThrowError(e);
        }
      } else {
        std::memcpy(retval,
          Buffer::Data(resBuf.ToLocalChecked()),
          info->GetResultSize());
        std::memcpy(parameters,
          Buffer::Data(argBuf.ToLocalChecked()),
          sizeof(char*) * info->GetArgc());
      }
    } else {
      Local<Value> errorFunctionArgv[1];
      static const char* outOfMemory = "ffi fatal: out of memory";
      errorFunctionArgv[0] = Nan::New<String>(outOfMemory).ToLocalChecked();
      info->GetErrorFunction()->Call(1, errorFunctionArgv);
    }
  }
}

void Callback::WatcherCallback(uv_async_t *w, int revents) {
  uv_mutex_lock(&g_queue_mutex);

  while (!g_queue.empty()) {
    ThreadedCallbackInvokation *inv = g_queue.front();
    g_queue.pop();

    DispatchToV8(inv->m_cbinfo, inv->m_retval, inv->m_parameters, true);
    inv->SignalDoneExecuting();
  }

  uv_mutex_unlock(&g_queue_mutex);
}

/**
 * decode callback_info from code buffer object.
 */
callback_info*
Callback::DecodeCallbackInfo(
    v8::Isolate* isolate,
    v8::Local<v8::Object>& codeBuffer)
{
    callback_info* result = nullptr;
    Local<Context> context = isolate->GetCurrentContext();
    v8::MaybeLocal<v8::Value> codeObjValue = codeBuffer->Get(context, 
        String::NewFromUtf8(isolate, "code").ToLocalChecked());

    if (!codeObjValue.IsEmpty()) {
        v8::Local<v8::Value> codeObjValue0 = codeObjValue.ToLocalChecked(); 
        if (codeObjValue0->IsObject()) {
            node_ffi::CodeObject* codeObj0; 
            v8::Local<v8::Object> codeObj = codeObjValue0->ToObject(
                context).ToLocalChecked();
            codeObj0 = Nan::ObjectWrap::Unwrap<node_ffi::CodeObject>(codeObj);
            result = codeObj0->callbackInfo;
        }
    }
    return result;
}


/*
 * Creates an `ffi_closure *` pointer around the given JS function. Returns the
 * executable C function pointer as a node Buffer instance.
 */

NAN_METHOD(Callback::NewCallback) {
  if (info.Length() != 5) {
    return Nan::ThrowError("Not enough arguments.");
  }

  // Args: cif pointer, JS function
  // TODO: Check args
  v8::Isolate *isolate = info.GetIsolate();
  v8::Local<Context> ctx = isolate->GetCurrentContext();
  ffi_cif *cif = (ffi_cif *)node_ffi::UnwrapPointer(isolate, info[0]);

#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))

  size_t resultSize = info[1]->Int32Value(ctx).FromJust();
  int argc = info[2]->Int32Value(ctx).FromJust();
#else 
  size_t resultSize = info[1]->Int32Value();
  int argc = info[2]->Int32Value();
#endif
  Local<Function> errorReportCallback = Local<Function>::Cast(info[3]);
  Local<Function> callback = Local<Function>::Cast(info[4]);
  ffi_status status;
  void *code;
  void* cbBuf;
  cbBuf = ffi_closure_alloc(sizeof(callback_info), &code);

  std::unique_ptr<Nan::Callback> errorFunction(
    new (std::nothrow) Nan::Callback(errorReportCallback));
  std::unique_ptr<Nan::Callback> function(
    new (std::nothrow) Nan::Callback(callback));

  if (!cbBuf || !function || !errorFunction) {
    return Nan::ThrowError("ffi_closure_alloc() Returned Error");
  }
  callback_info* cbInfo;
  cbInfo = new (cbBuf) callback_info();
  cbInfo->SetResultSize(resultSize);
  cbInfo->SetArgc(argc);
  cbInfo->SetErrorFunction(errorFunction);
  cbInfo->SetFunction(function);

  // store a reference to the callback function pointer
  // (not sure if this is actually needed...)
  cbInfo->SetCode(code);

  //Callback *self = new Callback(callback, closure, code, argc);

  status = ffi_prep_closure_loc(
    cbInfo,
    cif,
    Invoke,
    cbInfo,
    code
  );

  if (status != FFI_OK) {
    callback_info::Free(cbInfo);
    std::string strbuf("ffi_prep_closure() Returned Error "); 
    strbuf += status;
    Nan::ThrowError(strbuf.c_str());
    return;
  }

  Local<Object> codeBuff = Local<Object>::Cast(node_ffi::WrapPointer(isolate,
    (char*)code, sizeof(void*), true)); 
   
  node_ffi::CodeObject* codeObj;
  codeObj = new (std::nothrow) node_ffi::CodeObject(
        cbInfo);
  if (codeObj) {
    Local<ObjectTemplate> jsCodeObjTemp = v8::ObjectTemplate::New(isolate);
    jsCodeObjTemp->SetInternalFieldCount(1);
    Local<Object> jsCodeObj = jsCodeObjTemp->NewInstance(
      isolate->GetCurrentContext()).ToLocalChecked();
    codeObj->AttachTo(jsCodeObj);
    codeBuff->DefineOwnProperty(
        isolate->GetCurrentContext(),
        String::NewFromUtf8(isolate, "code").ToLocalChecked(),
        jsCodeObj,
        static_cast<PropertyAttribute>(
           PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete));
    info.GetReturnValue().Set(codeBuff);
  } else {
    callback_info::Free(cbInfo);

    Nan::ThrowError("outof memory");
  }
}

/*
 * This is the function that gets called when the C function pointer gets
 * executed.
 */

void Callback::Invoke(ffi_cif *cif, void *retval, void **parameters, void *user_data) {
  callback_info *info = reinterpret_cast<callback_info *>(user_data);

  // are we executing from another thread?
#ifdef WIN32
  if (g_threadID == GetCurrentThreadId()) {
#else
  uv_thread_t self_thread = (uv_thread_t) uv_thread_self();
  if (uv_thread_equal(&self_thread, &g_mainthread)) {
#endif
    DispatchToV8(info, retval, parameters);
  } else {
    // hold the event loop open while this is executing
#if NODE_VERSION_AT_LEAST(0, 7, 9)
    uv_ref((uv_handle_t *)&g_async);
#else
    uv_ref(uv_default_loop());
#endif

    // create a temporary storage area for our invokation parameters
    ThreadedCallbackInvokation *inv = new ThreadedCallbackInvokation(info, retval, parameters);

    // push it to the queue -- threadsafe
    uv_mutex_lock(&g_queue_mutex);
    g_queue.push(inv);
    uv_mutex_unlock(&g_queue_mutex);

    // send a message to our main thread to wake up the WatchCallback loop
    uv_async_send(&g_async);

    // wait for signal from calling thread
    inv->WaitForExecution();

#if NODE_VERSION_AT_LEAST(0, 7, 9)
    uv_unref((uv_handle_t *)&g_async);
#else
    uv_unref(uv_default_loop());
#endif
    delete inv;
  }
}

/*
 * Init stuff.
 */

void Callback::Initialize(Handle<Object> target) {
  Nan::HandleScope scope;
#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Local<Context> ctx = isolate->GetCurrentContext();
  Nan::Set(target, Nan::New<String>("Callback").ToLocalChecked(),
    Nan::New<FunctionTemplate>(NewCallback)->GetFunction(ctx).ToLocalChecked());
#else
  Nan::Set(target, Nan::New<String>("Callback").ToLocalChecked(),
    Nan::New<FunctionTemplate>(NewCallback)->GetFunction());
#endif

  // initialize our threaded invokation stuff
#ifdef WIN32
  g_threadID = GetCurrentThreadId();
#else
  g_mainthread = (uv_thread_t) uv_thread_self();
#endif
  uv_async_init(uv_default_loop(), &g_async, (uv_async_cb) Callback::WatcherCallback);
  uv_mutex_init(&g_queue_mutex);

  // allow the event loop to exit while this is running
#if NODE_VERSION_AT_LEAST(0, 7, 9)
  uv_unref((uv_handle_t *)&g_async);
#else
  uv_unref(uv_default_loop());
#endif
}

}
