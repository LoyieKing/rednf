// ZEN_LIB::shm_vector 还原声明（布局来自 gunnersvr DWARF，2026-08-10）
// shm_vector<T>：smem_base_(void*) / vector_head_(head*) / data_base_(T*)，sizeof = 12。
// 头结构 _shm_vector_head：head_magic_ / max_ / size_，sizeof = 12。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_VECTOR_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_VECTOR_H_H_

#include <stdint.h>
#include <stddef.h>

namespace ZEN_LIB {

struct _shm_vector_head { // sizeof = 12
    uint32_t head_magic_; // 沙箱/镜像区头标识（0x18000c）
    uint32_t max_;        // 元素上限（0x10000）
    uint32_t size_;       // 当前元素个数
};

template <typename T>
struct shm_vector { // sizeof = 16（多态：vptr@0；二进制 new 分配 0x10、data_base_@0xc）
    virtual ~shm_vector() {}     // vptr @0x00
    void *smem_base_;            // 0x04
    _shm_vector_head *vector_head_; // 0x08
    T *data_base_;               // 0x0c
};

} // namespace ZEN_LIB

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_VECTOR_H_H_
