#ifndef __NODE_FFI_WRAP_POINTER_H__
#define __NODE_FFI_WRAP_POINTER_H__

#include <v8.h>
#include <node.h>
#include <nan.h>

#ifdef __cplusplus

namespace node_ffi {

/*
 * Converts an arbitrary pointer to a node Buffer with 0-length
 */

void
wrap_pointer_cb(
    char *data,
    void *hint);

/**
 * convert pointer to v8 local value
 */
inline v8::Local<v8::Value>
WrapPointer(
    v8::Isolate* isolate,
    char *ptr, size_t length, bool external) {

    v8::Local<v8::Value> res;
    if (!external) {
        Nan::EscapableHandleScope scope;
        res = scope.Escape(
            Nan::NewBuffer(
                ptr, length, wrap_pointer_cb, NULL).ToLocalChecked());
    } else {
        Nan::EscapableHandleScope scope;
        v8::Local<v8::ArrayBuffer> ab = v8::ArrayBuffer::New(
            isolate, sizeof(void*));
        char** destPtr = reinterpret_cast<char **>(ab->Data());
        *destPtr = ptr;
        v8::MaybeLocal<v8::Uint8Array> bfPtr = node::Buffer::New(
            isolate, ab, 0, sizeof(void*));
        v8::Local<v8::Uint8Array> resArray;
        if (!bfPtr.IsEmpty()) {
            v8::Local<v8::Uint8Array> resArray0 = bfPtr.ToLocalChecked();
            v8::Maybe<bool> propRes = v8::Nothing<bool>();
            propRes = resArray0->DefineOwnProperty(
                isolate->GetCurrentContext(),
                v8::String::NewFromUtf8(isolate, "external").ToLocalChecked(),
                v8::Boolean::New(isolate, true),
                static_cast<v8::PropertyAttribute>(
                    v8::PropertyAttribute::ReadOnly
                    | v8::PropertyAttribute::DontDelete));
            if (propRes.FromJust()) {
                propRes = resArray0->DefineOwnProperty(
                    isolate->GetCurrentContext(),
                    v8::String::NewFromUtf8(
                        isolate, "indirection").ToLocalChecked(),
                    v8::Integer::New(isolate, 2),
                    static_cast<v8::PropertyAttribute>(
                        v8::PropertyAttribute::DontDelete));
            }
            if (propRes.FromJust()) {
                resArray = resArray0;
            }
        }
        res = scope.Escape(resArray);
    }
    return res;
}


inline v8::Local<v8::Value>
WrapPointer(
    v8::Isolate* isolate, char *ptr, bool external) {
    return WrapPointer(isolate, ptr, 0, external);
}

inline char*
UnwrapPointer(
    const v8::Local<v8::Value>& dataobj, bool external) {
    char* result = nullptr;
    if (dataobj->IsUint8Array()) {
        v8::Local<v8::Uint8Array> ptrArray = v8::Local<v8::Uint8Array>::Cast(
            dataobj);
        char *ptr = reinterpret_cast<char*>(ptrArray->Buffer()->Data()); 
        ptr += ptrArray->ByteOffset();
        if (!external) {
            result = ptr;
        } else {
            char** ptrRef = reinterpret_cast<char**>(ptr);
            result = *ptrRef;
        }
    }
    return result;
}


inline bool
IsExternalPtr(
    v8::Isolate* isolate,
    const v8::Local<v8::Value>& dataobj) {
    bool result = false;
    if (dataobj->IsObject()) {
        v8::Local<v8::Object> ptrObj = dataobj.As<v8::Object>(); 
        v8::MaybeLocal<v8::Value> externalValue;
        externalValue = ptrObj->Get(isolate->GetCurrentContext(), 
            v8::String::NewFromUtf8(isolate, "external").ToLocalChecked());
        if (!externalValue.IsEmpty()) {
            result = externalValue.ToLocalChecked()->IsTrue();
        }
   } 
   return result;
}

inline
char* UnwrapPointer(
    v8::Isolate* isolate,
    const v8::Local<v8::Value>& dataobj) {
    return UnwrapPointer(dataobj, IsExternalPtr(isolate, dataobj));
}

} 
#endif
#endif
