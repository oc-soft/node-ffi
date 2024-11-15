#include <node.h>
#include <nan.h>
#include "node-ffi.h"
#include "node-ffi/callback-info.h"

NAN_MODULE_INIT(init) {
  Nan::HandleScope scope;
  
  node_ffi::FFI::InitializeBindings(target);
  node_ffi::FFI::InitializeStaticFunctions(target);
  node_ffi::CallbackInfo::Initialize(target);
}

NODE_MODULE(ffi_bindings, init)
