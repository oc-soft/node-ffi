#include "node-ffi/constants.h"
#include "node-ffi/ffi-config.h"
#include <ffi.h>

#define SET_ENUM_VALUE(_value) \
  Nan::ForceSet(target, Nan::New<v8::String>(#_value).ToLocalChecked(), \
  Nan::New<v8::Integer>((uint32_t)_value), \
  static_cast<v8::PropertyAttribute>( \
    v8::PropertyAttribute::ReadOnly \
    | v8::PropertyAttribute::DontDelete))

namespace node_ffi {

/**
 * manage constants for javascript
 */
 
NAN_MODULE_INIT(Constants::Register)
{
    Nan::Set(target,
        Nan::New<v8::String>("version").ToLocalChecked(),
        Nan::New<v8::String>(
            node_ffi::FFIConfig::GetVersion()).ToLocalChecked());

    Nan::ForceSet(target,
        Nan::New<v8::String>("FFI_ARG_SIZE").ToLocalChecked(),
        Nan::New<v8::Uint32>((uint32_t)sizeof(ffi_arg)),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));
    Nan::ForceSet(target,
        Nan::New<v8::String>("FFI_SARG_SIZE").ToLocalChecked(),
        Nan::New<v8::Uint32>((uint32_t)sizeof(ffi_sarg)),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));
    Nan::ForceSet(target,
        Nan::New<v8::String>("FFI_TYPE_SIZE").ToLocalChecked(),
        Nan::New<v8::Uint32>((uint32_t)sizeof(ffi_type)),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));
    Nan::ForceSet(target,
        Nan::New<v8::String>("FFI_CIF_SIZE").ToLocalChecked(),
        Nan::New<v8::Uint32>((uint32_t)sizeof(ffi_cif)),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));

    // `ffi_status` enum values
    SET_ENUM_VALUE(FFI_OK);
    SET_ENUM_VALUE(FFI_BAD_TYPEDEF);
    SET_ENUM_VALUE(FFI_BAD_ABI);

    // `ffi_abi` enum values
    SET_ENUM_VALUE(FFI_DEFAULT_ABI);
    SET_ENUM_VALUE(FFI_FIRST_ABI);
    SET_ENUM_VALUE(FFI_LAST_ABI);
/* ---- ARM processors ---------- */
#ifdef __arm__
    SET_ENUM_VALUE(FFI_SYSV);
    SET_ENUM_VALUE(FFI_VFP);
/* ---- Intel x86 Win32 ---------- */
#elif defined(X86_WIN32)
    SET_ENUM_VALUE(FFI_SYSV);
    SET_ENUM_VALUE(FFI_STDCALL);
    SET_ENUM_VALUE(FFI_THISCALL);
    SET_ENUM_VALUE(FFI_FASTCALL);
    SET_ENUM_VALUE(FFI_MS_CDECL);
#elif defined(X86_WIN64)
    SET_ENUM_VALUE(FFI_WIN64);
#else
    /* ---- Intel x86 and AMD x86-64 - */
    SET_ENUM_VALUE(FFI_SYSV);
    /* Unix variants all use the same ABI for x86-64  */
    SET_ENUM_VALUE(FFI_UNIX64);
#endif

    bool hasObjc = false;
#if __OBJC__ || __OBJC2__
    hasObjc = true;
#endif
    Nan::ForceSet(target,
        Nan::New<v8::String>("HAS_OBJC").ToLocalChecked(),
        Nan::New<v8::Boolean>(hasObjc),
        static_cast<v8::PropertyAttribute>(
            v8::PropertyAttribute::ReadOnly
            | v8::PropertyAttribute::DontDelete));

}
// vi: se ts=4 sw=4 et:
}
