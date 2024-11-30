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
    ffi_cif *cif = reinterpret_cast<ffi_cif *>(
        node_ffi::UnwrapPointer(isolate, info[0]));
    void (*fn)(void) = FFI_FN(node_ffi::UnwrapPointer(isolate, info[1]));
    void* res = node_ffi::UnwrapPointer(isolate, info[2]);
    void** fnargs = reinterpret_cast<void**>(
        node_ffi::UnwrapPointer(isolate, info[3]));

#if __OBJC__ || __OBJC2__
    @try {
#endif
        ffi_call(cif, fn, res, fnargs);
#if __OBJC__ || __OBJC2__
    } @catch (id ex) {

        v8::Local<v8::Value> exValue = node_ffi::WrapPointer(
            isolate, reinterpret_cast<char*>(ex), true);

        if (!exValue.IsEmpty()) {
            v8::Local<v8::Value> exContainer;
            v8::Local<v8::Object> exObj = v8::Local<v8::Object>::Cast(exValue);
            exContainer = ObjcObjectWrap::New(isolate, ex).ToLocalChecked();
            v8::Maybe<bool> bState = v8::Nothing<bool>();
            bState = exObj->DefineOwnProperty(
                isolate->GetCurrentContext(),
                v8::String::NewFromUtf8(
                    isolate, "exceptionWrap").ToLocalChecked(),
                exContainer,
                static_cast<v8::PropertyAttribute>(
                    v8::PropertyAttribute::ReadOnly
                    | v8::PropertyAttribute::DontDelete));
            Nan::ThrowError(exValue);
        }
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
// vi: se ts=4 sw=4 et:
