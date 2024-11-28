#include <node.h>
#include <nan.h>
#include "node-ffi/prep.h"
#include "node-ffi/call.h"
#include "node-ffi/constants.h"
#include "node-ffi/dl.h"
#include "node-ffi/errno.h"
#include "node-ffi/callback.h"
#include "node-ffi/async-call.h"
#include "node-ffi/types.h"

NAN_MODULE_INIT(init) {
    Nan::HandleScope scope;
  
    node_ffi::Call::Register(target);
    node_ffi::Prep::Register(target);
    node_ffi::Constants::Register(target);
    node_ffi::AsyncCall::Register(target);
    node_ffi::Dl::Register(target);
    node_ffi::Types::Register(target);
    node_ffi::Callback::Initialize(target);
#ifndef _WIN32
    node_ffi::Errno::Register(target);
    // errno would be register with dynamic library for windows.
    // In windows environment, you might select one option from followings.
    // 1. ucrtbase.dll
    // 2. vcruntime<version>.dll
    // 3. msvc.dll 
    // In ffi_binding default windows c runtime is liked with libcrul.lib
    // static library. You can not get errno about functions from c runtime dll.
#endif
}

NAN_MODULE_WORKER_ENABLED(ffi_bindings, init)
