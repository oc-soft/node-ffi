#ifndef __NODE_FFI_TYPES_H__
#define __NODE_FFI_TYPES_H__

#include <nan.h>

#ifdef __cplusplus
namespace node_ffi {

/**
 * manage exposed types for javascript.
 */
class Types {
public:
    static NAN_MODULE_INIT(Register);
};

}

#endif
#endif
