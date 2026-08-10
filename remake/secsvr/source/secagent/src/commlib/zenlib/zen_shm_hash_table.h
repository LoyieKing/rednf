// Reconstructed from secagent disassembly (ZEN_LIB hash helpers)
// Original path: src/commlib/zenlib/zen_shm_hash_table.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_TABLE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_TABLE_H_H_

#include <stddef.h>
#include <stdint.h>

namespace ZEN_LIB {

// smem_hash<unsigned long long>（secagent 模板实参）。
// 反汇编核对：bucket = (uint32)(key & 0xffffffff) + (uint32)(key >> 32)，
// 再对 num_of_node_ 取模（shm_hashtable_expire::bkt_num_key 内联展开）。
template<typename KeyType>
struct smem_hash {
    size_t operator()(const KeyType &key) const {
        return (size_t)((uint32_t)key + (uint32_t)((uint64_t)key >> 32));
    }
};

} // namespace ZEN_LIB

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_TABLE_H_H_
