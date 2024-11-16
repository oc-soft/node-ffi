#ifndef __NODE_FFI_ERRNO_H__
#define __NODE_FFI_ERRNO_H__

#include <nan.h>

#ifdef __cplusplus

namespace node_ffi {
/**
 * errno related functions
 */
class Errno {
public:
    static NAN_MODULE_INIT(Register);
};

}

#endif

#endif
