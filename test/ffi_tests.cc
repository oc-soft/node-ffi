#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "v8.h"
#include "node.h"
#include "node_buffer.h"
#include <nan.h>

using namespace v8;
using namespace node;

/*
 * Exported function with C naming and calling conventions.
 * Used by dynamic_library.js to test symbol lookup.
 * Never actually called.
 */

extern "C"
int
NODE_MODULE_EXPORT
ExportedFunction(int value)
{
  return value * 2;
}

namespace {



char* UnwrapPointer(
  Isolate* isolate,
  const Local<Value>& dataobj);



/*
 * Test struct definition used in the test harness functions below.
 */

typedef struct box {
  int width;
  int height;
} _box;

/*
 * Accepts a struct by value, and returns a struct by value.
 */

box double_box(box input) {
  box rtn;
  // modify the input box, ensure on the JS side that it's not altered
  input.width *= 2;
  input.height *= 2;
  rtn.width = input.width;
  rtn.height = input.height;
  return rtn;
}

/*
 * Accepts a box struct pointer, and returns a struct by value.
 */

box double_box_ptr(box *input) {
  box rtn;
  // modify the input box, ensure on the JS side that IT IS altered
  input->width *= 2;
  input->height *= 2;
  rtn.width = input->width;
  rtn.height = input->height;
  return rtn;
}

/*
 * Accepts a struct by value, and returns an int.
 */

int area_box(box input) {
  return input.width * input.height;
}

/*
 * Accepts a box pointer and returns an int.
 */

int area_box_ptr(box *input) {
  return input->width * input->height;
}

/*
 * Creates a box and returns it by value.
 */

box create_box(int width, int height) {
  box rtn = { width, height };
  return rtn;
}

/*
 * Creates a box that has the sum of the width and height for its own values.
 */

box add_boxes(box boxes[], int num) {
  box rtn = { 0, 0 };
  box cur;
  for (int i = 0; i < num; i++) {
    cur = boxes[i];
    rtn.width += cur.width;
    rtn.height += cur.height;
  }
  return rtn;
}

/*
 * Reads "ints" from the "input" array until -1 is found.
 * Returns the number of elements in the array.
 */

int *int_array(int *input) {
  int *array = input;
  while (*array != -1){
    *array = *array * 2;
    array++;
  }
  return input;
}

/*
 * Tests for passing a Struct that contains Arrays inside of it.
 */

struct arst {
  int num;
  double array[20];
};

struct arst array_in_struct (struct arst input) {
  struct arst rtn;
  rtn.num = input.num * 2;
  for (int i = 0; i < 20; i++) {
    rtn.array[i] = input.array[i] * 3.14;
  }
  return rtn;
}

/*
 * Tests for C function pointers.
 */

typedef int (*my_callback)(int);

my_callback callback_func (my_callback cb) {
  return cb;
}

/*
 * Hard-coded `strtoul` binding, for the benchmarks.
 *
 * args[0] - the string number to convert to a real Number
 * args[1] - a "buffer" instance to write into (the "endptr")
 * args[2] - the base (0 means autodetect)
 */

NAN_METHOD(Strtoul) {
  Nan::HandleScope();
  int base;
  char **endptr;

  Nan::Utf8String buf(info[0]);

  Local<Value> endptr_arg = info[1];
  endptr = (char **)Buffer::Data(endptr_arg.As<Object>());
#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))
  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  v8::Local<Context> ctx = isolate->GetCurrentContext();
  base = info[2]->Int32Value(ctx).FromJust();
#else 
  base = info[2]->Int32Value();
#endif

  unsigned long val = strtoul(*buf, endptr, base);

  info.GetReturnValue().Set(Nan::New<Integer>((uint32_t)val));
}


// experiments for #72

static Global<Value> callback_container;


NAN_METHOD(SetCb) {
  Nan::HandleScope();
  callback_container.Reset(info.GetIsolate(), info[0]); 
  if (!callback_container.IsWeak()) {
    callback_container.SetWeak();
  }
  info.GetReturnValue().SetUndefined();
}


void CallCallbackInternal(
  Isolate* isolate) {
  v8::HandleScope scope(isolate);
  Local<Value> buffer_val = callback_container.Get(isolate);
  if (!buffer_val.IsEmpty() && buffer_val->IsObject()) {
    void (*callback)(void) = 0;
    Local<Object> buf_obj = buffer_val.As<Object>();
    callback = reinterpret_cast<void (*)()>(UnwrapPointer(isolate, buf_obj));
    if (callback) {
      callback(); 
    } else {
      isolate->ThrowError("callback container is reclaimed");
    }    
  } else {
    isolate->ThrowError("\"set_cb()\" is not called "
        "or callback container is reclaimed");
  }
}


NAN_METHOD(CallCb) {
  CallCallbackInternal(info.GetIsolate());
  info.GetReturnValue().SetUndefined();
}

// Race condition in threaded callback invocation testing
// https://github.com/node-ffi/node-ffi/issues/153
void play_ping_pong (const char* (*callback) (const char*)) {
  const char * response;
  do {
    response = callback("ping");
  } while (strcmp(response, "pong") == 0);
}


// https://github.com/node-ffi/node-ffi/issues/169
int test_169(char* dst, int len) {
  const char src[] = "sample str\0";
  strncpy(dst, src, len);
  return fmin(len, strlen(src));
}


// https://github.com/TooTallNate/ref/issues/56
struct Obj56 {
  bool traceMode;
};
int test_ref_56(struct Obj56 *obj) {
  return obj->traceMode ? 1 : 0;
}


void wrap_pointer_cb(char *data, void *hint) {
}

inline Local<Value> WrapPointer(
    Isolate* isolate,
    char *ptr,
    size_t length, bool external) {
    Nan::EscapableHandleScope scope;
    Local<Object> result;
    if (!external) {
        result = Nan::NewBuffer(
            ptr, length, wrap_pointer_cb, NULL).ToLocalChecked();
    } else {
        result = Buffer::New(isolate, sizeof(ptr)).ToLocalChecked();
        char** dstPtr = reinterpret_cast<char**>(Buffer::Data(result));
        *dstPtr = ptr; 
        Nan::DefineOwnProperty(result,
            String::NewFromUtf8(isolate, "external").ToLocalChecked(),
            Boolean::New(isolate, true),
            static_cast<PropertyAttribute>(
                PropertyAttribute::ReadOnly
                | PropertyAttribute::DontDelete));

    }
    return scope.Escape(result);
}

Local<Value> WrapPointer(Isolate* isolate, char *ptr, bool external) {
    return WrapPointer(isolate, ptr, 0, external);
}

char*
UnwrapPointer(
    const Local<Value>& dataobj,
    bool external) {
    char* result = nullptr;
    if (dataobj->IsUint8Array()) {
        Local<Uint8Array> ptrArray = Local<Uint8Array>::Cast(dataobj);
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


bool IsExternalPtr(Isolate* isolate, const Local<Value>& dataobj) {
  bool result = false;
  if (dataobj->IsObject()) {
    Local<Object> ptrObj = dataobj.As<Object>(); 
    
    MaybeLocal<Value> externalValue = ptrObj->Get(isolate->GetCurrentContext(), 
      String::NewFromUtf8(isolate, "external").ToLocalChecked());
    if (!externalValue.IsEmpty()) {
      result = externalValue.ToLocalChecked()->IsTrue();
    }
  } 
  return result;
}

char* UnwrapPointer(Isolate* isolate, const Local<Value>& dataobj) {
  return UnwrapPointer(dataobj, IsExternalPtr(isolate, dataobj));
}

 
void Initialize(v8::Local<v8::Object> target,
    v8::Local<v8::Value> module,
    void* priv) {
    Nan::HandleScope();

#if WIN32
  // initialize "floating point support" on Windows?!?!
  // (this is some serious bullshit...)
  // http://support.microsoft.com/kb/37507
    float x = 2.3f;
#endif
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Maybe<bool> bState = v8::Nothing<bool>(); 
 
    // atoi and abs here for testing purposes
    bState = target->Set(ctx, Nan::New<String>("atoi").ToLocalChecked(),
        WrapPointer(isolate, (char *)atoi, true));

    // Windows has multiple `abs` signatures,
    // so we need to manually disambiguate
    int (*absPtr)(int)(abs);
    bState = target->Set(ctx, Nan::New<String>("abs").ToLocalChecked(),
        WrapPointer(isolate, (char *)absPtr, true));

    // sprintf pointer; used in the varadic tests
    bState = target->Set(ctx,
        Nan::New<String>("sprintf").ToLocalChecked(),
        WrapPointer(isolate, (char *)sprintf, true));

    // hard-coded `strtoul` binding, for the benchmarks
#if defined(V8_MAJOR_VERSION) && (V8_MAJOR_VERSION > 4 ||                      \
  (V8_MAJOR_VERSION == 4 && defined(V8_MINOR_VERSION) && V8_MINOR_VERSION > 3))
    Nan::Set(target, Nan::New<String>("strtoul").ToLocalChecked(),
        Nan::New<FunctionTemplate>(Strtoul)->GetFunction(ctx).ToLocalChecked());

    Nan::Set(target, Nan::New<String>("set_cb").ToLocalChecked(),
        Nan::New<FunctionTemplate>(SetCb)->GetFunction(ctx).ToLocalChecked());
    Nan::Set(target, Nan::New<String>("call_cb").ToLocalChecked(),
        Nan::New<FunctionTemplate>(CallCb)->GetFunction(ctx).ToLocalChecked());
#else
    Nan::Set(target, Nan::New<String>("strtoul").ToLocalChecked(),
        Nan::New<FunctionTemplate>(Strtoul)->GetFunction());

    Nan::Set(target, Nan::New<String>("set_cb").ToLocalChecked(),
        Nan::New<FunctionTemplate>(SetCb)->GetFunction());
    Nan::Set(target, Nan::New<String>("call_cb").ToLocalChecked(),
        Nan::New<FunctionTemplate>(CallCb)->GetFunction());
#endif
    // also need to test these custom functions
    bState = target->Set(ctx,
        Nan::New<String>("double_box").ToLocalChecked(),
        WrapPointer(isolate, (char *)double_box, true));
    bState = target->Set(ctx,
        Nan::New<String>("double_box_ptr").ToLocalChecked(),
        WrapPointer(isolate, (char *)double_box_ptr, true));
    bState = target->Set(ctx, Nan::New<String>("area_box").ToLocalChecked(),
        WrapPointer(isolate, (char *)area_box, true));
    bState = target->Set(ctx, Nan::New<String>("area_box_ptr").ToLocalChecked(),
        WrapPointer(isolate, (char *)area_box_ptr, true));
    bState = target->Set(ctx, Nan::New<String>("create_box").ToLocalChecked(),
        WrapPointer(isolate, (char *)create_box, true));
    bState = target->Set(ctx, Nan::New<String>("add_boxes").ToLocalChecked(),
        WrapPointer(isolate, (char *)add_boxes, true));
    bState = target->Set(ctx, Nan::New<String>("int_array").ToLocalChecked(),
        WrapPointer(isolate, (char *)int_array, true));
    bState = target->Set(ctx,
        Nan::New<String>("array_in_struct").ToLocalChecked(),
        WrapPointer(isolate, (char *)array_in_struct, true));
    bState = target->Set(
        ctx, Nan::New<String>("callback_func").ToLocalChecked(),
        WrapPointer(isolate, (char *)callback_func, true));
    bState = target->Set(
        ctx, Nan::New<String>("play_ping_pong").ToLocalChecked(),
        WrapPointer(isolate, reinterpret_cast<char*>(play_ping_pong), true));
    bState = target->Set(ctx, Nan::New<String>("test_169").ToLocalChecked(),
        WrapPointer(isolate, reinterpret_cast<char*>(test_169), true));
    bState = target->Set(ctx, Nan::New<String>("test_ref_56").ToLocalChecked(),
        WrapPointer(isolate, reinterpret_cast<char*>(test_ref_56), true));
}

} // anonymous namespace

NODE_MODULE(ffi_tests, Initialize);
