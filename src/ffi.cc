#include <node.h>
#include <errno.h>
#include "node-ffi.h"
#include "objc-object-wrap.h"
#include "node-ffi/wrap-pointer.h"
#include "node-ffi/async-call.h"
#include "node-ffi/ffi-config.h"


using namespace node;
using namespace v8;

#define THROW_ERROR_EXCEPTION(x) Nan::ThrowError(x)
#define THROW_ERROR_EXCEPTION_WITH_STATUS_CODE(x, y) Nan::ThrowError(x)

#define FFI_ASYNC_ERROR (ffi_status)1

namespace node_ffi {


///////////////

#define SET_ENUM_VALUE(_value) \
  Nan::ForceSet(target, Nan::New<String>(#_value).ToLocalChecked(), \
  Nan::New<Integer>((uint32_t)_value), \
  static_cast<PropertyAttribute>(ReadOnly|DontDelete))

NAN_MODULE_INIT(FFI::InitializeBindings) {
}

}


