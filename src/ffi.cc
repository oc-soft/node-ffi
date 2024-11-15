#include <node.h>
#include <errno.h>
#include "node-ffi.h"
#include "objc-object-wrap.h"
#include "node-ffi/callback-info.h"
#include "node-ffi/wrap-pointer.h"
#include "node-ffi/async-call-params.h"
#include "node-ffi/ffi-config.h"

#ifdef WIN32
#include "win32-dlfcn.h"
#else
#include <dlfcn.h>
#endif


int node_ffi_errno() {
    return errno;
}

using namespace node;
using namespace v8;

#define THROW_ERROR_EXCEPTION(x) Nan::ThrowError(x)
#define THROW_ERROR_EXCEPTION_WITH_STATUS_CODE(x, y) Nan::ThrowError(x)

#define FFI_ASYNC_ERROR (ffi_status)1

namespace node_ffi {

///////////////

NAN_MODULE_INIT(FFI::InitializeStaticFunctions) {
  Local<Object> o = Nan::New<Object>();
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
 
  // dl functions used by the DynamicLibrary JS class
  o->Set(ctx, Nan::New<String>("dlopen").ToLocalChecked(),  node_ffi::WrapPointer(isolate, (char *)dlopen, true));
  o->Set(ctx, Nan::New<String>("dlclose").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)dlclose, true));
  o->Set(ctx, Nan::New<String>("dlsym").ToLocalChecked(),   node_ffi::WrapPointer(isolate, (char *)dlsym, true));
  o->Set(ctx, Nan::New<String>("dlerror").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)dlerror, true));

  o->Set(ctx, Nan::New<String>("_errno").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)node_ffi_errno, true));

  target->Set(ctx, Nan::New<String>("StaticFunctions").ToLocalChecked(), o);
}

///////////////

#define SET_ENUM_VALUE(_value) \
  Nan::ForceSet(target, Nan::New<String>(#_value).ToLocalChecked(), \
  Nan::New<Integer>((uint32_t)_value), \
  static_cast<PropertyAttribute>(ReadOnly|DontDelete))

NAN_MODULE_INIT(FFI::InitializeBindings) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  Nan::Set(target, Nan::New<String>("version").ToLocalChecked(),
    Nan::New<String>(node_ffi::FFIConfig::GetVersion()).ToLocalChecked());
#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))

  v8::Local<Context> ctx = isolate->GetCurrentContext();
  Nan::Set(target, Nan::New<String>("ffi_prep_cif").ToLocalChecked(),
    Nan::New<FunctionTemplate>(
      FFIPrepCif)->GetFunction(ctx).ToLocalChecked());
  Nan::Set(target, Nan::New<String>("ffi_prep_cif_var").ToLocalChecked(),
    Nan::New<FunctionTemplate>(
      FFIPrepCifVar)->GetFunction(ctx).ToLocalChecked());
  Nan::Set(target, Nan::New<String>("ffi_call").ToLocalChecked(),
    Nan::New<FunctionTemplate>(
      FFICall)->GetFunction(ctx).ToLocalChecked());
  Nan::Set(target, Nan::New<String>("ffi_call_async").ToLocalChecked(),
    Nan::New<FunctionTemplate>(
      FFICallAsync)->GetFunction(ctx).ToLocalChecked());
#else 
  // main function exports
  Nan::Set(target, Nan::New<String>("ffi_prep_cif").ToLocalChecked(),
    Nan::New<FunctionTemplate>(FFIPrepCif)->GetFunction());
  Nan::Set(target, Nan::New<String>("ffi_prep_cif_var").ToLocalChecked(),
    Nan::New<FunctionTemplate>(FFIPrepCifVar)->GetFunction());
  Nan::Set(target, Nan::New<String>("ffi_call").ToLocalChecked(),
    Nan::New<FunctionTemplate>(FFICall)->GetFunction());
  Nan::Set(target, Nan::New<String>("ffi_call_async").ToLocalChecked(),
    Nan::New<FunctionTemplate>(FFICallAsync)->GetFunction());
#endif

  // `ffi_status` enum values
  SET_ENUM_VALUE(FFI_OK);
  SET_ENUM_VALUE(FFI_BAD_TYPEDEF);
  SET_ENUM_VALUE(FFI_BAD_ABI);

  // `ffi_abi` enum values
  SET_ENUM_VALUE(FFI_DEFAULT_ABI);
  SET_ENUM_VALUE(FFI_FIRST_ABI);
  SET_ENUM_VALUE(FFI_LAST_ABI);
  /* ---- ARM processors ---------- */
#ifdef __arm__
  SET_ENUM_VALUE(FFI_SYSV);
  SET_ENUM_VALUE(FFI_VFP);
  /* ---- Intel x86 Win32 ---------- */
#elif defined(X86_WIN32)
  SET_ENUM_VALUE(FFI_SYSV);
  SET_ENUM_VALUE(FFI_STDCALL);
  SET_ENUM_VALUE(FFI_THISCALL);
  SET_ENUM_VALUE(FFI_FASTCALL);
  SET_ENUM_VALUE(FFI_MS_CDECL);
#elif defined(X86_WIN64)
  SET_ENUM_VALUE(FFI_WIN64);
#else
  /* ---- Intel x86 and AMD x86-64 - */
  SET_ENUM_VALUE(FFI_SYSV);
  /* Unix variants all use the same ABI for x86-64  */
  SET_ENUM_VALUE(FFI_UNIX64);
#endif

  /* flags for dlopen() */
#ifdef RTLD_LAZY
  SET_ENUM_VALUE(RTLD_LAZY);
#endif
#ifdef RTLD_NOW
  SET_ENUM_VALUE(RTLD_NOW);
#endif
#ifdef RTLD_LOCAL
  SET_ENUM_VALUE(RTLD_LOCAL);
#endif
#ifdef RTLD_GLOBAL
  SET_ENUM_VALUE(RTLD_GLOBAL);
#endif
#ifdef RTLD_NOLOAD
  SET_ENUM_VALUE(RTLD_NOLOAD);
#endif
#ifdef RTLD_NODELETE
  SET_ENUM_VALUE(RTLD_NODELETE);
#endif
#ifdef RTLD_FIRST
  SET_ENUM_VALUE(RTLD_FIRST);
#endif

  /* flags for dlsym() */
#ifdef RTLD_NEXT
  Nan::ForceSet(target,Nan::New<String>("RTLD_NEXT").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)RTLD_NEXT, true), static_cast<PropertyAttribute>(ReadOnly | DontDelete));
#endif
#ifdef RTLD_DEFAULT
  Nan::ForceSet(target,Nan::New<String>("RTLD_DEFAULT").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)RTLD_DEFAULT, true), static_cast<PropertyAttribute>(ReadOnly | DontDelete));
#endif
#ifdef RTLD_SELF
  Nan::ForceSet(target,Nan::New<String>("RTLD_SELF").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)RTLD_SELF, true), static_cast<PropertyAttribute>(ReadOnly|DontDelete));
#endif
#ifdef RTLD_MAIN_ONLY
  Nan::ForceSet(target,Nan::New<String>("RTLD_MAIN_ONLY").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)RTLD_MAIN_ONLY, true), static_cast<PropertyAttribute>(ReadOnly|DontDelete));
#endif

  Nan::ForceSet(target,Nan::New<String>("FFI_ARG_SIZE").ToLocalChecked(), Nan::New<Uint32>((uint32_t)sizeof(ffi_arg)), static_cast<PropertyAttribute>(ReadOnly|DontDelete));
  Nan::ForceSet(target,Nan::New<String>("FFI_SARG_SIZE").ToLocalChecked(), Nan::New<Uint32>((uint32_t)sizeof(ffi_sarg)), static_cast<PropertyAttribute>(ReadOnly | DontDelete));
  Nan::ForceSet(target,Nan::New<String>("FFI_TYPE_SIZE").ToLocalChecked(), Nan::New<Uint32>((uint32_t)sizeof(ffi_type)), static_cast<PropertyAttribute>(ReadOnly | DontDelete));
  Nan::ForceSet(target,Nan::New<String>("FFI_CIF_SIZE").ToLocalChecked(), Nan::New<Uint32>((uint32_t)sizeof(ffi_cif)), static_cast<PropertyAttribute>(ReadOnly | DontDelete));

  bool hasObjc = false;
#if __OBJC__ || __OBJC2__
  hasObjc = true;
#endif
  Nan::ForceSet(target,Nan::New<String>("HAS_OBJC").ToLocalChecked(), Nan::New<Boolean>(hasObjc), static_cast<PropertyAttribute>(ReadOnly | DontDelete));

  Local<Object> ftmap = Nan::New<Object>();
  ftmap->Set(ctx, Nan::New<String>("void").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_void, true));
  ftmap->Set(ctx, Nan::New<String>("uint8").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint8, true));
  ftmap->Set(ctx, Nan::New<String>("int8").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint8, true));
  ftmap->Set(ctx, Nan::New<String>("uint16").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint16, true));
  ftmap->Set(ctx, Nan::New<String>("int16").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint16, true));
  ftmap->Set(ctx, Nan::New<String>("uint32").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint32, true));
  ftmap->Set(ctx, Nan::New<String>("int32").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint32, true));
  ftmap->Set(ctx, Nan::New<String>("uint64").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint64, true));
  ftmap->Set(ctx, Nan::New<String>("int64").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint64, true));
  ftmap->Set(ctx, Nan::New<String>("uchar").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_uchar, true));
  ftmap->Set(ctx, Nan::New<String>("char").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_schar, true));
  ftmap->Set(ctx, Nan::New<String>("ushort").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_ushort, true));
  ftmap->Set(ctx, Nan::New<String>("short").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_sshort, true));
  ftmap->Set(ctx, Nan::New<String>("uint").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint, true));
  ftmap->Set(ctx, Nan::New<String>("int").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint, true));
  ftmap->Set(ctx, Nan::New<String>("float").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_float, true));
  ftmap->Set(ctx, Nan::New<String>("double").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_double, true));
  ftmap->Set(ctx, Nan::New<String>("pointer").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_pointer, true));
  // NOTE: "long" and "ulong" get handled in JS-land
  // Let libffi handle "long long"
  ftmap->Set(ctx, Nan::New<String>("ulonglong").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_ulong, true));
  ftmap->Set(ctx, Nan::New<String>("longlong").ToLocalChecked(), node_ffi::WrapPointer(isolate, (char *)&ffi_type_slong, true));

  target->Set(ctx, Nan::New<String>("FFI_TYPES").ToLocalChecked(), ftmap);
}

/*
 * Function that creates and returns an `ffi_cif` pointer from the given return
 * value type and argument types.
 *
 * args[0] - the CIF buffer
 * args[1] - the number of args
 * args[2] - the "return type" pointer
 * args[3] - the "arguments types array" pointer
 * args[4] - the ABI to use
 *
 * returns the ffi_status result from ffi_prep_cif()
 */

NAN_METHOD(FFI::FFIPrepCif) {
  unsigned int nargs;
  char *rtype, *cif;
  void **atypes;
  ffi_abi abi;

  if (info.Length() != 5) {
    return THROW_ERROR_EXCEPTION("ffi_prep_cif() requires 5 arguments!");
  }

  Handle<Value> cif_buf = info[0];
  if (!Buffer::HasInstance(cif_buf)) {
    return THROW_ERROR_EXCEPTION("prepCif(): Buffer required as first arg");
  }

  v8::Isolate *isolate = info.GetIsolate();

  cif = node_ffi::UnwrapPointer(isolate, cif_buf);

  v8::Local<Context> ctx = isolate->GetCurrentContext();
  nargs = info[1]->Uint32Value(ctx).FromJust();
  rtype = node_ffi::UnwrapPointer(isolate, info[2]);
  atypes = reinterpret_cast<void**>(node_ffi::UnwrapPointer(isolate, info[3]));
  int res;
  res = 0;

  abi = (ffi_abi)info[4]->Uint32Value(ctx).FromJust();
  res = ffi_prep_cif(
          (ffi_cif *)cif,
          abi,
          nargs,
          (ffi_type *)rtype,
          (ffi_type **)atypes);
  info.GetReturnValue().Set(Nan::New<Integer>(res));
}

/*
 * Function that creates and returns an `ffi_cif` pointer from the given return
 * value type and argument types.
 *
 * args[0] - the CIF buffer
 * args[1] - the number of fixed args
 * args[2] - the number of total args
 * args[3] - the "return type" pointer
 * args[4] - the "arguments types array" pointer
 * args[5] - the ABI to use
 *
 * returns the ffi_status result from ffi_prep_cif_var()
 */
NAN_METHOD(FFI::FFIPrepCifVar) {
  unsigned int fargs, targs;
  char *rtype, *atypes, *cif;
  ffi_status status;
  ffi_abi abi;

  if (info.Length() != 6) {
    return THROW_ERROR_EXCEPTION("ffi_prep_cif() requires 5 arguments!");
  }

  Handle<Value> cif_buf = info[0];
  if (!Buffer::HasInstance(cif_buf)) {
    return THROW_ERROR_EXCEPTION("prepCifVar(): Buffer required as first arg");
  }

  v8::Isolate *isolate = info.GetIsolate();
  cif = node_ffi::UnwrapPointer(isolate, cif_buf);

#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))
  v8::Local<Context> ctx = isolate->GetCurrentContext();
  fargs = info[1]->Uint32Value(ctx).FromJust();
  targs = info[2]->Uint32Value(ctx).FromJust();
  rtype = node_ffi::UnwrapPointer(isolate, info[3]);
  atypes = node_ffi::UnwrapPointer(isolate, info[4]);
  abi = (ffi_abi)info[5]->Uint32Value(ctx).FromJust();
#else
  fargs = info[1]->Uint32Value();
  targs = info[2]->Uint32Value();
  rtype = node_ffi::UnwrapPointer(isolate, info[3]);
  atypes = node_ffi::UnwrapPointer(isolate, info[4]);
  abi = (ffi_abi)info[5]->Uint32Value();
#endif

  status = ffi_prep_cif_var(
      (ffi_cif *)cif,
      abi,
      fargs,
      targs,
      (ffi_type *)rtype,
      (ffi_type **)atypes);

  info.GetReturnValue().Set(Nan::New<Integer>(status));
}

/*
 * JS wrapper around `ffi_call()`.
 *
 * args[0] - Buffer - the `ffi_cif *`
 * args[1] - Buffer - the C function pointer to invoke
 * args[2] - Buffer - the `void *` buffer big enough to hold the return value
 * args[3] - Buffer - the `void **` array of pointers containing the arguments
 */

NAN_METHOD(FFI::FFICall) {
  if (info.Length() != 4) {
    return THROW_ERROR_EXCEPTION("ffi_call() requires 4 arguments!");
  }
  v8::Isolate *isolate = info.GetIsolate();
  char *cif = node_ffi::UnwrapPointer(isolate, info[0]);
  char *fn = node_ffi::UnwrapPointer(isolate, info[1]);
  char *res = node_ffi::UnwrapPointer(isolate, info[2]);
  char *fnargs = node_ffi::UnwrapPointer(isolate, info[3]);

#if __OBJC__ || __OBJC2__
    @try {
#endif
      ffi_call(
          (ffi_cif *)cif,
          FFI_FN(fn),
          (void *)res,
          (void **)fnargs
        );
#if __OBJC__ || __OBJC2__
    } @catch (id ex) {
      return THROW_ERROR_EXCEPTION(ObjcObjectWrap::New(isolate, ex, true));
    }
#endif

  info.GetReturnValue().SetUndefined();
}

/*
 * Asynchronous JS wrapper around `ffi_call()`.
 *
 * args[0] - Buffer - the `ffi_cif *`
 * args[1] - Buffer - the C function pointer to invoke
 * args[2] - Buffer - the `void *` buffer big enough to hold the return value
 * args[3] - Buffer - the `void **` array of pointers containing the arguments
 * args[4] - Function - the callback function to invoke when complete
 */

NAN_METHOD(FFI::FFICallAsync) {
  if (info.Length() != 5) {
    return THROW_ERROR_EXCEPTION("ffi_call_async() requires 5 arguments!");
  }

  AsyncCallParams *p = new (std::nothrow) AsyncCallParams();
  uv_work_t *req = new (std::nothrow) uv_work_t;

  std::unique_ptr<Nan::Callback> callback;
  callback = std::unique_ptr<Nan::Callback>(
    new (std::nothrow) Nan::Callback(Local<Function>::Cast(info[4])));

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
      FFI::AsyncFFICall,
      (uv_after_work_cb)FFI::FinishAsyncFFICall);

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
void FFI::AsyncFFICall(uv_work_t *req) {
  AsyncCallParams *p = reinterpret_cast<AsyncCallParams *>(req->data);

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

void FFI::FinishAsyncFFICall(uv_work_t *req) {
  Nan::HandleScope scope;

  AsyncCallParams *p = reinterpret_cast<AsyncCallParams *>(req->data);

  Local<Value> argv[] = { Nan::Null() };
#if __OBJC__ || __OBJC2__
  if (p->HasErr()) {
    Isolate* isolate = Isolate::GetCurrent();
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

}


