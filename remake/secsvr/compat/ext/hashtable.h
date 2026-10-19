// Compatibility shim for the GCC 4.x libstdc++ header removed in modern GCC.
// The secsvr sources include <ext/hashtable.h>; the hash containers are still
// available through <ext/hash_map> / <ext/hash_set>.
#ifndef SECSVR_COMPAT_EXT_HASHTABLE_H_
#define SECSVR_COMPAT_EXT_HASHTABLE_H_

#include <ext/hash_map>
#include <ext/hash_set>

#endif  // SECSVR_COMPAT_EXT_HASHTABLE_H_
