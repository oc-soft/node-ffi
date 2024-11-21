#ifndef __NODE_FFI_PREP_H__
#define __NODE_FFI_PREP_H__

#include "nan.h"

#ifdef __cplusplus

namespace node_ffi {

class Prep {
public:
    static NAN_MODULE_INIT(Register);
    static NAN_METHOD(CifVar);
    static NAN_METHOD(Cif);
};

}

#endif
// vi: se ts=4 sw=4 et:
#endif
