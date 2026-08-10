// Reconstructed from secagent binary DWARF + disassembly (2026-08-10)
// Original path: src/commlib/zenlib/zen_shm_predefine.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_PREDEFINE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_PREDEFINE_H_H_

#include <stddef.h>

extern const size_t _INVALID_POINT; // line 408
namespace ZEN_LIB {

#ifndef ZEN_SHM_PREDEFINE_H_SHM_MEMORY_BASE_DEFINED_
#define ZEN_SHM_PREDEFINE_H_SHM_MEMORY_BASE_DEFINED_

// 共享内存基类（secagent 二进制为多态：vptr + smem_base_，8 字节）
class _shm_memory_base {
public:
    static const size_t _INVALID_POINT = (size_t)-1; // line 408
    virtual ~_shm_memory_base() {}
    _shm_memory_base() : smem_base_(NULL) {}
    _shm_memory_base(char *smem_base) : smem_base_(smem_base) {}
protected:
    char *smem_base_; // offset 4
};

#endif // ZEN_SHM_PREDEFINE_H_SHM_MEMORY_BASE_DEFINED_

    extern const size_t NUM_HASH_SAFE_PRIMES_LIST; // line 90
    extern const size_t NUM_PRIMES_LIST_ELEMENT; // line 98
} // namespace ZEN_LIB

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_PREDEFINE_H_H_
