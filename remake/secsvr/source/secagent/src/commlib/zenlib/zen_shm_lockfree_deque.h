// Reconstructed from DWARF (gunnersvr, decl_file 266 = zen_shm_lockfree_deque.h)
// Original path: src/commlib/zenlib/zen_shm_lockfree_deque.h
// Layout verified against binary member offsets (see .cpp header comments).
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_LOCKFREE_DEQUE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_LOCKFREE_DEQUE_H_H_

#include <cstddef>

#include "src/commlib/zenlib/zen_os_adapt_predefine.h" // ZEN_THREAD_ID
#include "src/commlib/zenlib/zen_shm_predefine.h" // _shm_memory_base

namespace ZEN_LIB {

struct dequechunk_node { // line 21, byte_size 4 (flexible trailing array)
    static const size_t MIN_SIZE_DEQUE_CHUNK_NODE = 5; // line 24
    static const size_t DEQUECHUNK_NODE_HEAD_LEN = 4; // line 26

    size_t size_of_node_; // line 36, offset 0
    char chunkdata[]; // line 43, offset 4 (flexible array member)

    void fillin(dequechunk_node* node, size_t nodelen, char* data); // line 50
    void* operator new(size_t arg0, size_t nodelen); // line 52
};

struct shm_dequechunk; // forward declaration (friend below)

struct _shm_dequechunk_head { // line 66, byte_size 20
    friend struct shm_dequechunk;
protected:
    size_t size_of_mmap_; // line 71, offset 0
    size_t size_of_deque_; // line 74, offset 4
    size_t max_len_node_; // line 77, offset 8
    volatile size_t deque_begin_; // line 81, offset 12
    volatile size_t deque_end_; // line 83, offset 16

    _shm_dequechunk_head(); // line 86 (declared only)
    ~_shm_dequechunk_head(); // line 95 (declared only)
};

struct shm_dequechunk : public _shm_memory_base { // line 104, byte_size 28
protected:
    static const size_t JUDGE_FULL_INTERVAL = 8; // line 107

    _shm_dequechunk_head* dequechunk_head_; // line 112, offset 8
    char* dequechunk_database_; // line 115, offset 12
    dequechunk_node* line_wrap_nodeptr_; // line 118, offset 16
    bool if_check_pthread_; // line 121, offset 20
    ZEN_THREAD_ID pthread_id_; // line 122, offset 24

    shm_dequechunk& operator=(const shm_dequechunk&); // line 128 (declared only)
    void snap_getpoint(size_t& pstart, size_t& pend); // line 132
    void deque_lock(); // line 135 (declared only)
    void deque_unlock(); // line 137 (declared only)
    shm_dequechunk(); // line 142

public:
    static size_t getallocsize(size_t szdeque); // line 149
    static shm_dequechunk* initialize(size_t size_of_deque,
                                      size_t max_len_node, char* pmmap,
                                      bool if_restore,
                                      bool if_check_pthread); // line 157
    void clear(); // line 160
    bool push_end(const dequechunk_node* node); // line 163
    bool pop_front(dequechunk_node* node); // line 167
    bool read_front(dequechunk_node* node); // line 169
    bool pop_front_new(dequechunk_node*& new_node); // line 172
    bool read_front_new(dequechunk_node*& new_node); // line 174
    bool read_front_ptr(const dequechunk_node*& node_ptr); // line 177
    bool discard_frond(); // line 180
    size_t get_front_len(); // line 183 (defined inline in the .cpp, line 199;
                            // fully inlined at -O2, no emitted symbol)
    size_t freesize(); // line 186
    bool empty(); // line 189
    bool full(); // line 191

    ~shm_dequechunk(); // line 144（公开：ZEN_Bus_MMAPPipe 释放管道时调用）
};

} // namespace ZEN_LIB

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_LOCKFREE_DEQUE_H_H_
