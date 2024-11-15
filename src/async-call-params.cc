#include "node-ffi/async-call-params.h"

#if __OBJC__ || __OBJC2__
#include "objc-object-wrap.h"
#endif

AsyncCallParams::~AsyncCallParams()
{
#if __OBJC__ || __OBJC2__
    SetErr(Nil);
#endif
}

#if __OBJC__ || __OBJC2__

bool
AsyncCallParams::HasErr() const
{
    return GetErrRef() != Nil;
}

id
AsyncCallParams::GetErrRef() const
{
    return err;
}

void
AsyncCallParams::SetErr(
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
AsyncCallParams::GetErr(
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
AsyncCallParams::SetCif(
    ffi_cif* cif)
{
    this->cif = cif;
}

ffi_cif*
AsyncCallParams::GetCif() const
{
    return cif;
}

void
(*AsyncCallParams::GetFn() const)(void)
{
    return fn;
}

void
AsyncCallParams::SetFn(
    void (*fn)(void))
{
    this->fn = fn;
}

void*
AsyncCallParams::GetRes() const
{
    return res;
}

void
AsyncCallParams::SetRes(
    void* res)
{
    this->res = res;
}

void**
AsyncCallParams::GetArgv() const
{
    return argv;
}

void
AsyncCallParams::SetArgv(
    void** argv)
{
    this->argv = argv;
}

void
AsyncCallParams::SetCallback(
    std::unique_ptr<Nan::Callback>& callback)
{
    std::unique_ptr<Nan::Callback> tmpValue;
    std::swap(tmpValue, callback);
    std::swap(tmpValue, this->callback);
}

std::unique_ptr<Nan::Callback>&
AsyncCallParams::GetCallback() 
{
    return callback;
}


// vi: se ts=4 sw=4 et:
