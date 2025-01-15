#include "node-ffi/prep.h"
#include <v8.h>
#include <ffi.h>
#include <node.h>
#include "node-ffi/wrap-pointer.h"
namespace node_ffi {

#define THROW_ERROR_EXCEPTION(x) Nan::ThrowError(x)
#define THROW_ERROR_EXCEPTION_WITH_STATUS_CODE(x, y) Nan::ThrowError(x)


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

NAN_METHOD(Prep::Cif) {
  unsigned int nargs;
  char *rtype, *cif;
  void **atypes;
  ffi_abi abi;

  if (info.Length() != 5) {
    return THROW_ERROR_EXCEPTION("ffi_prep_cif() requires 5 arguments!");
  }

  v8::Local<v8::Value> cif_buf = info[0];
  if (!node::Buffer::HasInstance(cif_buf)) {
    return THROW_ERROR_EXCEPTION("prepCif(): Buffer required as first arg");
  }

  v8::Isolate *isolate = info.GetIsolate();

  cif = node_ffi::UnwrapPointer(isolate, cif_buf);

  v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
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
  info.GetReturnValue().Set(Nan::New<v8::Integer>(res));
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
NAN_METHOD(Prep::CifVar) {
  unsigned int fargs, targs;
  char *rtype, *atypes, *cif;
  ffi_status status;
  ffi_abi abi;

  if (info.Length() != 6) {
    THROW_ERROR_EXCEPTION("ffi_prep_cif() requires 5 arguments!");
        return;
  }

  v8::Local<v8::Value> cif_buf = info[0];
  if (!node::Buffer::HasInstance(cif_buf)) {
        THROW_ERROR_EXCEPTION("prepCifVar(): Buffer required as first arg");
        return;
  }

  v8::Isolate *isolate = info.GetIsolate();
  cif = node_ffi::UnwrapPointer(isolate, cif_buf);

#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))
  v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
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

  info.GetReturnValue().Set(Nan::New<v8::Integer>(status));
}

NAN_MODULE_INIT(Prep::Register) {
  v8::Isolate *isolate = v8::Isolate::GetCurrent();

#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))

    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    Nan::Set(target,
        Nan::New<v8::String>("ffi_prep_cif").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(
            Cif)->GetFunction(ctx).ToLocalChecked());
    Nan::Set(target,
        Nan::New<v8::String>("ffi_prep_cif_var").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(
            CifVar)->GetFunction(ctx).ToLocalChecked());
#else 
    // main function exports
    Nan::Set(target,
        Nan::New<v8::String>("ffi_prep_cif").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(Cif)->GetFunction());
    Nan::Set(target,
        Nan::New<v8::String>("ffi_prep_cif_var").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(CifVar)->GetFunction());
#endif

}

}
// vi: se ts=4 sw=4 et:
