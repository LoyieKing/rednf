// SocketChannelBus, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel_bus.cpp
//
// ORIG DWARF: sizeof(SocketChannelBus) == 12
//   +0x00 ZEN_Thread_MT_Bus_Pipe *bus_pipe_
//   +0x04 unsigned char           *send_bus_pop_buf_
//   +0x08 unsigned char           *recv_bus_pop_buf_
//
// ZEN_Thread_MT_Bus_Pipe has no out-of-line constructor in 394: init() raw-
// allocates 0x54 bytes and placement-constructs both ZEN_Thread_Mutex members.
//   +0x00 char*              database_
//   +0x04 size_t             recv_chunk_size_  (0x80000)
//   +0x08 size_t             send_chunk_size_  (0x80000)
//   +0x0c size_t             recv_alloc_size_
//   +0x10 size_t             send_alloc_size_
//   +0x14 shm_dequechunk*    recv_deque_
//   +0x18 shm_dequechunk*    send_deque_
//   +0x1c ZEN_Thread_Mutex   recv_lock_
//   +0x38 ZEN_Thread_Mutex   send_lock_
#ifndef TSS_RECON_COMM_SOCKET_CHANNEL_BUS_H_
#define TSS_RECON_COMM_SOCKET_CHANNEL_BUS_H_

#include <pthread.h>
#include <stddef.h>

#include "src/commlib/zenlib/zen_recon_support.h"
#include "src/commlib/zenlib/zen_shm_models.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/commlib/zenlib/zen_lock_guard.h"

namespace ZEN_LIB {

struct _shm_dequechunk_head {
    size_t size_of_mmap_;
    size_t size_of_deque_;
    size_t max_len_node_;
    volatile size_t deque_begin_;
    volatile size_t deque_end_;
};

struct dequechunk_node {            // ORIG sizeof 4, flexible trailing array
    unsigned int size_of_node_;     // +0x00
    char chunkdata[];               // +0x04, ORIG DWARF size 0
};

struct shm_dequechunk : public _shm_memory_base { // ORIG sizeof 24
    _shm_dequechunk_head *dequechunk_head_;   // +0x04
    char *dequechunk_database_;               // +0x08
    dequechunk_node *line_wrap_nodeptr_;      // +0x0c
    bool if_check_pthread_;                   // +0x10
    ZEN_THREAD_ID pthread_id_;                // +0x14 (ORIG: ZEN_THREAD_ID)

    static size_t getallocsize(unsigned int chunk_size);
    static shm_dequechunk *initialize(unsigned int chunk_size,
                                      unsigned int max_len_node, char *data,
                                      bool if_check, bool another);
    bool empty();
    bool push_end(const dequechunk_node *node);
    bool pop_front(dequechunk_node *node);
    // ORIG symbol: _ZN7ZEN_LIB14shm_dequechunk8freesizeEv (0x001ab260).
    unsigned int freesize();
};

} // namespace ZEN_LIB

// ZEN_Thread_Mutex is declared by src/commlib/zenlib/zen_lock_thread_mutex.h
// (included above); ORIG size 28 = ZEN_Lock_Base + pthread_mutex_t lock_ @4.

struct ZEN_Thread_MT_Bus_Pipe {
    char *database_;                       // +0x00
    size_t recv_chunk_size_;               // +0x04
    size_t send_chunk_size_;               // +0x08
    size_t recv_alloc_size_;               // +0x0c
    size_t send_alloc_size_;               // +0x10
    ZEN_LIB::shm_dequechunk *recv_deque_;  // +0x14
    ZEN_LIB::shm_dequechunk *send_deque_;  // +0x18
    ZEN_Thread_Mutex recv_lock_;           // +0x1c
    ZEN_Thread_Mutex send_lock_;           // +0x38
};

// ORIG DWARF: SocketChannelBus size 12 = ZEN_NON_Copyable base @0,
// bus_pipe_ @0, send_bus_pop_buf_ @4, recv_bus_pop_buf_ @8.
class SocketChannelBus : public ZEN_NON_Copyable {
public:
    SocketChannelBus();
    ~SocketChannelBus();

    int init();
    int fini();

    int push_recv_bus(void *node);
    int pop_recv_bus(void *node);
    int push_send_bus(const void *node);
    int pop_send_bus(void *&node);
    int peek_send_bus(size_t &node_size);

private:
    ZEN_Thread_MT_Bus_Pipe *bus_pipe_;
    unsigned char *send_bus_pop_buf_;
    unsigned char *recv_bus_pop_buf_;
};

#endif // TSS_RECON_COMM_SOCKET_CHANNEL_BUS_H_
