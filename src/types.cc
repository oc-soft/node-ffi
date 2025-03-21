#include "node-ffi/types.h"
#include <ffi.h>
#include "node-ffi/wrap-pointer.h"


namespace node_ffi {


NAN_MODULE_INIT(Types::Register)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> ftmap = Nan::New<v8::Object>();
    v8::Maybe<bool> state = v8::Nothing<bool>();

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("void").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_void, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("uint8").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint8, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("int8").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint8, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("uint16").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint16, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("int16").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint16, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("uint32").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint32, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("int32").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint32, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("uint64").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint64, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("int64").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint64, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("uchar").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_uchar, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("char").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_schar, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("ushort").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_ushort, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("short").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_sshort, true));

    state = ftmap->Set(ctx,
        Nan::New<v8::String>("uint").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_uint, true));
    state = ftmap->Set(ctx,
        Nan::New<v8::String>("int").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_sint, true));
    state = ftmap->Set(ctx,
        Nan::New<v8::String>("float").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_float, true));
    state = ftmap->Set(ctx,
        Nan::New<v8::String>("double").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_double, true));
    state = ftmap->Set(ctx,
        Nan::New<v8::String>("pointer").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_pointer, true));
    // NOTE: "long" and "ulong" get handled in JS-land
    // Let libffi handle "long long"
    state = ftmap->Set(ctx,
        Nan::New<v8::String>("ulonglong").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_ulong, true));
    state = ftmap->Set(ctx,
        Nan::New<v8::String>("longlong").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)&ffi_type_slong, true));
    state = target->Set(ctx,
        Nan::New<v8::String>("FFI_TYPES").ToLocalChecked(), ftmap);
}

}
