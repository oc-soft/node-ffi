#ifndef __NODE_FFI_CONSTANTS_H__
#define __NODE_FFI_CONSTANTS_H__

#include <nan.h>
#ifdef __cplusplus
namespace node_ffi {

/**
 * manage constants for javascript
 */
class Constants {
public:
    static NAN_MODULE_INIT(Register);
};

}
#endif
// vi: se ts=4 sw=4 et:
#endif
