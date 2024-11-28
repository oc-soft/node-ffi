#include "node-ffi/errno-i.h"
#include <cerrno>


namespace node_ffi {
#ifndef _WIN32
/**
 * set errno
 */
void
SetErrno(
    int err)
{
    errno = err;
}
#endif
}



// vi: se ts=4 sw=4 et:
