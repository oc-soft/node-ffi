#include "node-ffi/dl.h"
#include "node-ffi/wrap-pointer.h"

#ifndef WIN32
#include <dlfcn.h>
#else
#include "win32-dlfcn.h"
#endif


namespace node_ffi {

#define SET_ENUM_VALUE(_value) \
  Nan::DefineOwnProperty(target, \
    Nan::New<v8::String>(#_value).ToLocalChecked(), \
    Nan::New<v8::Integer>((uint32_t)_value), \
    static_cast<v8::PropertyAttribute>( \
        v8::PropertyAttribute::ReadOnly \
        | v8::PropertyAttribute::DontDelete))


NAN_MODULE_INIT(Dl::Register)
{
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    // dl functions used by the DynamicLibrary JS class
    v8::Local<v8::Object> o = Nan::New<v8::Object>();
    v8::Maybe<bool> state = v8::Nothing<bool>();
    state = o->Set(ctx,
        Nan::New<v8::String>("dlopen").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)dlopen, true));
    state = o->Set(ctx,
        Nan::New<v8::String>("dlclose").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)dlclose, true));
    state = o->Set(ctx,
        Nan::New<v8::String>("dlsym").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)dlsym, true));
    state = o->Set(ctx,
        Nan::New<v8::String>("dlerror").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)dlerror, true));
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
    Nan::DefineOwnProperty(target,
        Nan::New<v8::String>("RTLD_NEXT").ToLocalChecked(),
        node_ffi::WrapPointer(isolate,
            (char *)RTLD_NEXT, true),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));
#endif
#ifdef RTLD_DEFAULT
    Nan::DefineOwnProperty(target,
        Nan::New<v8::String>("RTLD_DEFAULT").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)RTLD_DEFAULT, true),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));
#endif
#ifdef RTLD_SELF
    Nan::DefineOwnProperty(target,
        Nan::New<v8::String>("RTLD_SELF").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)RTLD_SELF, true),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));
#endif
#ifdef RTLD_MAIN_ONLY
    Nan::DefineOwnProperty(target,
        Nan::New<v8::String>("RTLD_MAIN_ONLY").ToLocalChecked(),
        node_ffi::WrapPointer(isolate, (char *)RTLD_MAIN_ONLY, true),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));
#endif
    state = target->Set(ctx, Nan::New<v8::String>("Dl").ToLocalChecked(), o);
}

}
