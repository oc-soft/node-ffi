#include "node-ffi/call.h"

#include <ffi.h>
#include "node-ffi/wrap-pointer.h"
#include "node-ffi/objc-object-wrap.h"

namespace node_ffi {

/*
 * JS wrapper around `ffi_call()`.
 *
 * args[0] - Buffer - the `ffi_cif *`
 * args[1] - Buffer - the C function pointer to invoke
 * args[2] - Buffer - the `void *` buffer big enough to hold the return value
 * args[3] - Buffer - the `void **` array of pointers containing the arguments
 */

NAN_METHOD(Call::Invoke) {
  if (info.Length() != 4) {
    Nan::ThrowError("ffi_call() requires 4 arguments!");
    return;
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

NAN_MODULE_INIT(Call::Register) {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();

#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))

    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        Nan::Set(target,
        Nan::New<v8::String>("ffi_call").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(
            Invoke)->GetFunction(ctx).ToLocalChecked());
#else 
  // main function exports
    Nan::Set(target,
        Nan::New<v8::String>("ffi_call").ToLocalChecked(),
        Nan::New<v8::FunctionTemplate>(Invoke)->GetFunction());
#endif

}

}
