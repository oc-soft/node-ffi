#include "node-ffi/errno.h"
#include <cerrno>
#include "node-ffi/wrap-pointer.h"
#include "node-ffi/errno-i.h"


namespace node_ffi {


void
GetErrno(
    v8::Local<v8::String> name,
    const Nan::PropertyCallbackInfo<v8::Value>& cbInfo)
{
    if (sizeof(int) <= sizeof(int32_t)) {
        cbInfo.GetReturnValue().Set((int32_t)errno);
    } else {
        cbInfo.GetReturnValue().Set((double)errno);
    }
}

void
SetErrno(
    v8::Local<v8::String> name,
    v8::Local<v8::Value> value,
    const Nan::PropertyCallbackInfo<void>& cbInfo)
{
    v8::Local<v8::Context> ctx = cbInfo.GetIsolate()->GetCurrentContext();
    if (sizeof(int) <= sizeof(int32_t)) {
        v8::Maybe<int32_t> valueInt = value->Int32Value(ctx);
        if (valueInt.IsJust()) {
            SetErrno(valueInt.FromJust());
        }
    } else {
        v8::Maybe<int64_t> valueInt = value->IntegerValue(ctx);
        if (valueInt.IsJust()) {
            SetErrno(static_cast<int>(valueInt.FromJust()));
        }
    }
}

NAN_MODULE_INIT(Errno::Register)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Local<v8::ObjectTemplate> objTmp = Nan::New<v8::ObjectTemplate>();


    Nan::SetAccessor(
        objTmp,
        Nan::New<v8::String>("errno").ToLocalChecked(),
        GetErrno,
        SetErrno);
    v8::Local<v8::Object> o = Nan::NewInstance(objTmp).ToLocalChecked();

        
    v8::Maybe<bool> state = target->Set(ctx,
        Nan::New<v8::String>("Errno").ToLocalChecked(), o);
    state = v8::Nothing<bool>();
}

}
