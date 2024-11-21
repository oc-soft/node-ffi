#ifndef __NODE_FFI_H__
#define __NODE_FFI_H__
#include <ffi.h>
#include <uv.h>
#include <nan.h>

#ifdef __cplusplus

namespace node_ffi {

class FFI {
  public:
    static NAN_MODULE_INIT(InitializeBindings);

  protected:
};

}

#endif
#endif
