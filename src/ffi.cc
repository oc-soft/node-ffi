#include <node.h>
#include <errno.h>
#include "node-ffi.h"
#include "objc-object-wrap.h"
#include "node-ffi/wrap-pointer.h"
#include "node-ffi/async-call.h"
#include "node-ffi/ffi-config.h"


using namespace node;
using namespace v8;

#define THROW_ERROR_EXCEPTION(x) Nan::ThrowError(x)
#define THROW_ERROR_EXCEPTION_WITH_STATUS_CODE(x, y) Nan::ThrowError(x)

#define FFI_ASYNC_ERROR (ffi_status)1

namespace node_ffi {


///////////////

#define SET_ENUM_VALUE(_value) \
  Nan::ForceSet(target, Nan::New<String>(#_value).ToLocalChecked(), \
  Nan::New<Integer>((uint32_t)_value), \
  static_cast<PropertyAttribute>(ReadOnly|DontDelete))

NAN_MODULE_INIT(FFI::InitializeBindings) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();

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
    Nan::New<v8::FunctionTemplate>(
      FFICall)->GetFunction(ctx).ToLocalChecked());
#else 
  // main function exports
  Nan::Set(target, Nan::New<String>("ffi_prep_cif").ToLocalChecked(),
    Nan::New<FunctionTemplate>(FFIPrepCif)->GetFunction());
  Nan::Set(target, Nan::New<String>("ffi_prep_cif_var").ToLocalChecked(),
    Nan::New<FunctionTemplate>(FFIPrepCifVar)->GetFunction());
  Nan::Set(target, Nan::New<String>("ffi_call").ToLocalChecked(),
    Nan::New<FunctionTemplate>(FFICall)->GetFunction());
#endif

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

}


