#include "node-ffi/errno-i.h"
#include <stdlib.h>


namespace node_ffi {
#ifdef _WIN32
/**
 * set errno
 */
void
SetErrno(
    int err)
{
    _set_errno(err);
}
#endif
}



// vi: se ts=4 sw=4 et:
