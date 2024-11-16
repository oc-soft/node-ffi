#include "node-ffi/errno.h"
#include <cerrno>
#include "node-ffi/wrap-pointer.h"

namespace node_ffi {

static int
GetErrno() {
    return errno;
}

NAN_MODULE_INIT(Errno::Register)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Local<v8::Object> o = Nan::New<v8::Object>();
    o->Set(ctx,
        Nan::New<v8::String>("_errno").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)GetErrno, true));

    target->Set(ctx,
        Nan::New<v8::String>("Errno").ToLocalChecked(), o);
}

}
