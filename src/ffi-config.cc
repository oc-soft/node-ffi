#include "node-ffi/ffi-config.h"
#include "fficonfig.h"

namespace node_ffi {

/**
 * ffi library version
 */
const char*
FFIConfig::GetVersion()
{
    return PACKAGE_VERSION;
}
}
