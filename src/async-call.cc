#include "node-ffi/async-call.h"

#if __OBJC__ || __OBJC2__
#include "objc-object-wrap.h"
#endif

namespace node_ffi {

AsyncCall::~AsyncCall()
{
#if __OBJC__ || __OBJC2__
    SetErr(Nil);
#endif
}

#if __OBJC__ || __OBJC2__

bool
AsyncCall::HasErr() const
{
    return GetErrRef() != Nil;
}

id
AsyncCall::GetErrRef() const
{
    return err;
}

void
AsyncCall::SetErr(
    id err)
{
    if (err) {
        [err retain];
    }
    if (this->err) {
        [this->err release];
    }
    this->err = err;
}

MaybeLocal<v8::Object>
AsyncCall::GetErr(
    Isolate* isolate)
{
    MaybeLocal<v8::Object> result;
    if (err) {
        result = ObjcObjectWrap::New(isolate, err); 
    }
    return result;
}
#endif

void
AsyncCall::SetCif(
    ffi_cif* cif)
{
    this->cif = cif;
}

ffi_cif*
AsyncCall::GetCif() const
{
    return cif;
}

void
(*AsyncCall::GetFn() const)(void)
{
    return fn;
}

void
AsyncCall::SetFn(
    void (*fn)(void))
{
    this->fn = fn;
}

void*
AsyncCall::GetRes() const
{
    return res;
}

void
AsyncCall::SetRes(
    void* res)
{
    this->res = res;
}

void**
AsyncCall::GetArgv() const
{
    return argv;
}

void
AsyncCall::SetArgv(
    void** argv)
{
    this->argv = argv;
}

void
AsyncCall::SetCallback(
    std::unique_ptr<Nan::Callback>& callback)
{
    std::unique_ptr<Nan::Callback> tmpValue;
    std::swap(tmpValue, callback);
    std::swap(tmpValue, this->callback);
}

std::unique_ptr<Nan::Callback>&
AsyncCall::GetCallback() 
{
    return callback;
}

}
// vi: se ts=4 sw=4 et:
