// Compatibility shim for glibc's removed <libio.h>.
// The secsvr sources include it as part of the original 2010-era include set;
// they only need the stdio declarations in this tree.
#ifndef SECSVR_COMPAT_LIBIO_H_
#define SECSVR_COMPAT_LIBIO_H_

#include <stdio.h>

#endif  // SECSVR_COMPAT_LIBIO_H_
