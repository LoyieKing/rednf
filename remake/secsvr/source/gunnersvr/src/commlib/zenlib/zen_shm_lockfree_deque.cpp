// Reconstructed from gunnersvr binary (DWARF + objdump), semantic parity with:
// /data/secci/ci/jenkins/workspace/g3_release_suse32/src/commlib/zenlib/zen_shm_lockfree_deque.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux), 32-bit, -O2.
//
// Function order follows the original source line numbers (operator new is the
// first function in the TU so the _GLOBAL__I_ key matches the binary).
//
// Ring-buffer layout (mmap):
//   [0] size_of_mmap_   = getallocsize(size_of_deque) = size_of_deque + 28
//   [4] size_of_deque_  = size_of_deque + 8  (ring capacity)
//   [8] max_len_node_
//   [12] deque_begin_   (volatile index)
//   [16] deque_end_     (volatile index)
//   [20] ring data (size_of_deque_ bytes)
// 8 bytes of the ring are reserved (JUDGE_FULL_INTERVAL) so begin==end can
// never occur in a non-empty deque; freesize() reports raw_free - 8.
// A node is size_of_node_ bytes: 4-byte header (size_of_node_) + chunkdata.
// No CAS is used; "lock-free" refers to volatile index reads (snap_getpoint).

#include "src/commlib/zenlib/zen_shm_lockfree_deque.h"
#include "src/commlib/zenlib/zen_shm_predefine.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_os_adapt_thread.h"
#include "src/commlib/zenlib/zen_os_adapt_spin.h"
#include "src/commlib/zenlib/zen_predefine.h"

#include <cstdlib>
#include <cstring>
#include <new>
#include <iostream>

// zen_os_adapt_thread.h is still a stub in this workspace; declare the one
// ZEN_OS function this TU needs. An identical redeclaration from the restored
// header later is legal, so this fallback can stay.
#ifndef ZEN_OS_ADAPT_THREAD_H_PTHREAD_SELF_DECLARED_
#define ZEN_OS_ADAPT_THREAD_H_PTHREAD_SELF_DECLARED_
namespace ZEN_OS {
ZEN_THREAD_ID pthread_self();
}
#endif

// line 44
void* ZEN_LIB::dequechunk_node::operator new(size_t arg0, size_t nodelen) {
    void* ptr;
    if (nodelen < 4) {
        nodelen = 4;
    }
    ptr = new char[nodelen];
    *(size_t*)ptr = nodelen;
    return ptr;
}

// line 82
ZEN_LIB::shm_dequechunk::shm_dequechunk()
    : _shm_memory_base(), dequechunk_head_(0), dequechunk_database_(0),
      line_wrap_nodeptr_(0) {}

// line 89
ZEN_LIB::shm_dequechunk::~shm_dequechunk() {
    if (line_wrap_nodeptr_) {
        delete line_wrap_nodeptr_;
        line_wrap_nodeptr_ = 0;
    }
}

// line 99
size_t ZEN_LIB::shm_dequechunk::getallocsize(size_t szdeque) {
    return szdeque + sizeof(_shm_dequechunk_head) +
           2 * dequechunk_node::DEQUECHUNK_NODE_HEAD_LEN;
}

// line 123
ZEN_LIB::shm_dequechunk* ZEN_LIB::shm_dequechunk::initialize(
        size_t size_of_deque, size_t max_len_node, char* pmmap,
        bool if_restore, bool if_check_pthread) {
    if (size_of_deque <= sizeof(_shm_dequechunk_head)) {
        return 0;
    }
    _shm_dequechunk_head* dequechunk_head = (_shm_dequechunk_head*)pmmap;
    if (if_restore) {
        if (dequechunk_head->size_of_mmap_ != getallocsize(size_of_deque) ||
            dequechunk_head->size_of_deque_ !=
                size_of_deque + 2 * dequechunk_node::DEQUECHUNK_NODE_HEAD_LEN ||
            dequechunk_head->max_len_node_ != max_len_node) {
            return 0;
        }
    }
    dequechunk_head->size_of_mmap_ = getallocsize(size_of_deque);
    dequechunk_head->size_of_deque_ =
        size_of_deque + 2 * dequechunk_node::DEQUECHUNK_NODE_HEAD_LEN;
    dequechunk_head->max_len_node_ = max_len_node;

    shm_dequechunk* dequechunk = new shm_dequechunk();
    dequechunk->smem_base_ = pmmap;
    dequechunk->dequechunk_head_ = dequechunk_head;
    dequechunk->dequechunk_database_ = pmmap + sizeof(_shm_dequechunk_head);
    if (!if_restore) {
        dequechunk->clear();
    }
    dequechunk->if_check_pthread_ = if_check_pthread;
    if (if_check_pthread) {
        dequechunk->pthread_id_ = ZEN_OS::pthread_self();
    }
    return dequechunk;
}

// line 176
void ZEN_LIB::shm_dequechunk::clear() {
    dequechunk_head_->deque_begin_ = 0;
    dequechunk_head_->deque_end_ = 0;
    memset(dequechunk_database_, 0, dequechunk_head_->size_of_deque_);
}

// line 198
void ZEN_LIB::shm_dequechunk::snap_getpoint(size_t& pstart, size_t& pend) {
    pstart = dequechunk_head_->deque_begin_;
    pend = dequechunk_head_->deque_end_;
}

// line 199 (declared in header at line 183; fully inlined, no emitted symbol)
inline size_t ZEN_LIB::shm_dequechunk::get_front_len() {
    char* tmp1 = dequechunk_database_;
    size_t tmplen;
    char* tmp2 = dequechunk_database_ + dequechunk_head_->size_of_deque_;
    if (tmp1 + dequechunk_head_->deque_begin_ + 4 <= tmp2) {
        tmplen = *(size_t*)(tmp1 + dequechunk_head_->deque_begin_);
    } else {
        char* p = tmp1 + dequechunk_head_->deque_begin_;
        for (size_t i = 0; i < dequechunk_node::DEQUECHUNK_NODE_HEAD_LEN;
             ++i) {
            if (p >= tmp2) {
                p = tmp1;
            }
            ((char*)&tmplen)[i] = *p++;
        }
    }
    return tmplen;
}

// line 219
bool ZEN_LIB::shm_dequechunk::push_end(const dequechunk_node* node) {
    if (if_check_pthread_) {
        if (pthread_id_ != ZEN_OS::pthread_self()) {
            abort();
        }
    }
    size_t nodelen = node->size_of_node_;
    if (nodelen <= dequechunk_node::DEQUECHUNK_NODE_HEAD_LEN ||
        nodelen > dequechunk_head_->max_len_node_) {
        return false;
    }
    if (freesize() < nodelen) {
        return false;
    }
    char* pend = dequechunk_database_ + dequechunk_head_->deque_end_;
    size_t first = dequechunk_head_->size_of_deque_ -
                   dequechunk_head_->deque_end_;
    if (pend + nodelen <
        dequechunk_database_ + dequechunk_head_->size_of_deque_) {
        memcpy(pend, node, nodelen);
        dequechunk_head_->deque_end_ += nodelen;
        return true;
    }
    size_t second = nodelen - first;
    memcpy(pend, node, first);
    memcpy(dequechunk_database_, (char*)node + first, second);
    dequechunk_head_->deque_end_ = second;
    return true;
}

// line 279
bool ZEN_LIB::shm_dequechunk::pop_front(dequechunk_node* node) {
    if (if_check_pthread_) {
        if (pthread_id_ != ZEN_OS::pthread_self()) {
            abort();
        }
    }
    if (empty()) {
        return false;
    }
    char* pbegin = dequechunk_database_ + dequechunk_head_->deque_begin_;
    size_t tmplen = get_front_len();
    if (pbegin + tmplen <=
        dequechunk_database_ + dequechunk_head_->size_of_deque_) {
        memcpy(node, pbegin, tmplen);
        dequechunk_head_->deque_begin_ += node->size_of_node_;
        return true;
    }
    size_t first = dequechunk_head_->size_of_deque_ -
                   dequechunk_head_->deque_begin_;
    size_t second = tmplen - first;
    memcpy(node, pbegin, first);
    memcpy(dequechunk_database_, (char*)node + first, second);
    dequechunk_head_->deque_begin_ = second;
    return true;
}

// line 341
bool ZEN_LIB::shm_dequechunk::pop_front_new(dequechunk_node*& new_node) {
    if (empty()) {
        return false;
    }
    size_t tmplen = get_front_len();
    new_node = (dequechunk_node*)dequechunk_node::operator new(4, tmplen);
    return pop_front(new_node);
}

// line 373
bool ZEN_LIB::shm_dequechunk::read_front(dequechunk_node* node) {
    if (empty()) {
        return false;
    }
    char* pbegin = dequechunk_database_ + dequechunk_head_->deque_begin_;
    size_t tmplen = get_front_len();
    if (pbegin + tmplen <=
        dequechunk_database_ + dequechunk_head_->size_of_deque_) {
        memcpy(node, pbegin, tmplen);
        return true;
    }
    size_t first = dequechunk_head_->size_of_deque_ -
                   dequechunk_head_->deque_begin_;
    size_t second = tmplen - first;
    memcpy(node, pbegin, first);
    memcpy((char*)node + first, dequechunk_database_, second);
    return true;
}

// line 405
bool ZEN_LIB::shm_dequechunk::read_front_new(dequechunk_node*& new_node) {
    if (empty()) {
        return false;
    }
    size_t tmplen = get_front_len();
    new_node = (dequechunk_node*)dequechunk_node::operator new(4, tmplen);
    return read_front(new_node);
}

// line 422
bool ZEN_LIB::shm_dequechunk::read_front_ptr(
        const dequechunk_node*& node_ptr) {
    if (empty()) {
        return false;
    }
    char* pbegin = dequechunk_database_ + dequechunk_head_->deque_begin_;
    size_t tmplen = get_front_len();
    if (pbegin + tmplen <=
        dequechunk_database_ + dequechunk_head_->size_of_deque_) {
        node_ptr = (const dequechunk_node*)pbegin;
        return true;
    }
    size_t first = dequechunk_head_->size_of_deque_ -
                   dequechunk_head_->deque_begin_;
    if (!line_wrap_nodeptr_) {
        line_wrap_nodeptr_ =
            (dequechunk_node*)dequechunk_node::operator new(
                4, dequechunk_head_->max_len_node_);
    }
    memcpy(line_wrap_nodeptr_, pbegin, first);
    memcpy((char*)line_wrap_nodeptr_ + first, dequechunk_database_,
           tmplen - first);
    node_ptr = line_wrap_nodeptr_;
    return true;
}

// line 461
bool ZEN_LIB::shm_dequechunk::discard_frond() {
    if (empty()) {
        return false;
    }
    char* pbegin = dequechunk_database_ + dequechunk_head_->deque_begin_;
    size_t tmplen = get_front_len();
    if (pbegin + tmplen <=
        dequechunk_database_ + dequechunk_head_->size_of_deque_) {
        dequechunk_head_->deque_begin_ += tmplen;
        return true;
    }
    size_t first = dequechunk_head_->size_of_deque_ -
                   dequechunk_head_->deque_begin_;
    size_t second = tmplen - first;
    dequechunk_head_->deque_begin_ = second;
    return true;
}

// line 503
size_t ZEN_LIB::shm_dequechunk::freesize() {
    size_t pstart;
    size_t pend;
    snap_getpoint(pstart, pend);
    size_t szfree;
    if (pstart == pend) {
        szfree = dequechunk_head_->size_of_deque_;
    } else if (pstart < pend) {
        szfree = dequechunk_head_->size_of_deque_ - (pend - pstart);
    } else {
        szfree = pstart - pend;
    }
    return szfree - JUDGE_FULL_INTERVAL;
}

// line 532
bool ZEN_LIB::shm_dequechunk::empty() {
    return freesize() ==
           dequechunk_head_->size_of_deque_ - JUDGE_FULL_INTERVAL;
}

// line 538
bool ZEN_LIB::shm_dequechunk::full() {
    return freesize() == 0;
}
