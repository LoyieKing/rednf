// Compatibility shim for the GCC 4.x libstdc++ header removed in modern GCC.
// The secsvr sources include <ext/hash_fun.h>, <ext/hash_map>, <ext/hash_set>
// and <ext/hashtable.h>; modern libstdc++ still ships the containers but no
// hash_fun.h. Including hash_map provides the __gnu_cxx hash functors used by
// those translation units.
#ifndef SECSVR_COMPAT_EXT_HASH_FUN_H_
#define SECSVR_COMPAT_EXT_HASH_FUN_H_

#include <ext/hash_map>
#include <ext/hash_set>

#endif  // SECSVR_COMPAT_EXT_HASH_FUN_H_
