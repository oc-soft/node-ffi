#ifndef __NODE_FFI_CALL_H__
#define __NODE_FFI_CALL_H__

#include "nan.h"

#ifdef __cplusplus

namespace node_ffi {

class Call {
public:
    static NAN_MODULE_INIT(Register);
    static NAN_METHOD(Invoke);
};
}

#endif
// vi: se ts=4 sw=4 et:
#endif
