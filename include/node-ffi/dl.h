#ifndef __NODE_FFI_DL_H__
#define __NODE_FFI_DL_H__

#include <node.h>
#include <nan.h>
#ifdef __cplusplus

namespace node_ffi {

/**
 * dynamic library related function
 */
class Dl {

public:
    /**
     * register dynamic functions
     */
    static NAN_MODULE_INIT(Register);
};

}

#endif

#endif
