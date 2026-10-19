// Reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel_bus.cpp
// Address range 0x000d83e0-0x000d8b01.
//
// ORIG quirks preserved:
//   * fini() never releases bus_pipe_ or a non-NULL send_bus_pop_buf_; it only
//     calls operator delete when the buffer pointer is already NULL.
//   * pop_recv_bus() returns the *negation* of shm_dequechunk::pop_front()'s
//     bool (push_recv_bus returns 0 on success), see 0x000d891e.
//   * peek_send_bus() peeks a 4-byte node size out of the send deque and
//     reproduces the original wrap-around byte copy when the value straddles
//     the end of the chunk database.
#include "src/commlib/framework/comm_socket_channel_bus.h"

#include <new>

// ORIG CU static-init block (audit/report/init_orig.json): __ioinit only.
#include "src/ioinit_block.h"

namespace {

const char *const SOCKET_BUS_INIT_PIPE_FAIL =
    "[zenlib] ZEN_Thread_Bus_Pipe::initialize pipe fail recv[%p]size[%u],"
    "send[%p],size[%u].";
const char *const SOCKET_BUS_INIT_FAIL =
    "[socket channel bus]bus_pipe_->initialize failed, ret = %d";
const char *const SOCKET_BUS_NEW_PIPE_FAIL =
    "[socket channel bus]new ZEN_Thread_MT_Bus_Pipe() failed";
const char *const SOCKET_BUS_NEW_POP_BUF_FAIL =
    "[socket channel bus]new send_bus_pop_buf_ failed";
const char *const SOCKET_BUS_PUSH_RECV_FAIL =
    "[socket channel bus]bus_pipe_->push_back_recvpipe failed, ret = %d";
const char *const SOCKET_BUS_PUSH_SEND_FAIL =
    "[socket channel bus]bus_pipe_->push_back_sendpipe failed, ret = %d";

const size_t SOCKET_CHANNEL_BUS_CHUNK_SIZE = 0x80000;
const size_t SOCKET_CHANNEL_BUS_MAX_NODE = 0x10000;
const size_t SOCKET_CHANNEL_BUS_POP_BUF_SIZE = 0x10000;

} // namespace
namespace {

// ORIG uses an RAII lock scope: the landing pads around pop_front/push_end
// call the virtual unlock before _Unwind_Resume (see 0x000d880f).  The guard
// type is ZEN_Lock_Guard<ZEN_Thread_Mutex> (src/commlib/zenlib/zen_lock_guard.h).
typedef ZEN_Lock_Guard<ZEN_Thread_Mutex> SocketChannelLockGuard;

} // namespace


SocketChannelBus::SocketChannelBus() : send_bus_pop_buf_(NULL) {
}

SocketChannelBus::~SocketChannelBus() {
    fini();
}

int SocketChannelBus::fini() {
    if (bus_pipe_ == NULL) {
        bus_pipe_ = NULL;
    }
    if (send_bus_pop_buf_ == NULL) {
        delete send_bus_pop_buf_;
        send_bus_pop_buf_ = NULL;
    }
    return 0;
}

int SocketChannelBus::init() {
    ZEN_Thread_MT_Bus_Pipe *pipe = (ZEN_Thread_MT_Bus_Pipe *)
        ::operator new(sizeof(ZEN_Thread_MT_Bus_Pipe), std::nothrow);
    if (pipe == NULL) {
        bus_pipe_ = NULL;
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_BUS_NEW_PIPE_FAIL);
        return -1;
    }

    pipe->database_ = NULL;
    new (&pipe->recv_lock_) ZEN_Thread_Mutex((const char *)0);
    new (&pipe->send_lock_) ZEN_Thread_Mutex((const char *)0);
    pipe->recv_alloc_size_ = 0;
    pipe->recv_deque_ = NULL;
    pipe->send_alloc_size_ = 0;
    pipe->send_deque_ = NULL;

    bus_pipe_ = pipe;
    bus_pipe_->recv_chunk_size_ = SOCKET_CHANNEL_BUS_CHUNK_SIZE;
    bus_pipe_->send_chunk_size_ = SOCKET_CHANNEL_BUS_CHUNK_SIZE;
    bus_pipe_->recv_alloc_size_ =
        ZEN_LIB::shm_dequechunk::getallocsize(SOCKET_CHANNEL_BUS_CHUNK_SIZE);
    bus_pipe_->send_alloc_size_ =
        ZEN_LIB::shm_dequechunk::getallocsize(bus_pipe_->send_chunk_size_);

    bus_pipe_->database_ = new char[bus_pipe_->recv_alloc_size_
                                    + bus_pipe_->send_alloc_size_ + 0x20];
    bus_pipe_->recv_deque_ = ZEN_LIB::shm_dequechunk::initialize(
        bus_pipe_->recv_chunk_size_, SOCKET_CHANNEL_BUS_MAX_NODE,
        bus_pipe_->database_, false, false);
    bus_pipe_->send_deque_ = ZEN_LIB::shm_dequechunk::initialize(
        bus_pipe_->send_chunk_size_, SOCKET_CHANNEL_BUS_MAX_NODE,
        bus_pipe_->database_ + bus_pipe_->recv_alloc_size_ + 0x10, false,
        false);

    if (bus_pipe_->recv_deque_ == NULL || bus_pipe_->send_deque_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_BUS_INIT_PIPE_FAIL,
                                       bus_pipe_->recv_deque_,
                                       bus_pipe_->recv_chunk_size_,
                                       bus_pipe_->send_deque_,
                                       bus_pipe_->send_chunk_size_);
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_BUS_INIT_FAIL, -1);
        return -1;
    }

    send_bus_pop_buf_ = new (std::nothrow) unsigned char
        [SOCKET_CHANNEL_BUS_POP_BUF_SIZE];
    if (send_bus_pop_buf_ == NULL) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_BUS_NEW_POP_BUF_FAIL);
        return -1;
    }
    return 0;
}

int SocketChannelBus::push_recv_bus(void *node) {
    bool ret;
    {
        SocketChannelLockGuard guard(bus_pipe_->recv_lock_);
        ret = bus_pipe_->recv_deque_->push_end(
            (const ZEN_LIB::dequechunk_node *)node);
    }
    if (!ret) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_BUS_PUSH_RECV_FAIL, ret);
        return 1;
    }
    return 0;
}

int SocketChannelBus::pop_recv_bus(void *node) {
    if (node == NULL) {
        return -1;
    }
    bool ret;
    {
        SocketChannelLockGuard guard(bus_pipe_->recv_lock_);
        ret = bus_pipe_->recv_deque_->pop_front(
            (ZEN_LIB::dequechunk_node *)node);
    }
    return ret ? 0 : 1;
}

int SocketChannelBus::push_send_bus(const void *node) {
    bool ret;
    {
        SocketChannelLockGuard guard(bus_pipe_->send_lock_);
        ret = bus_pipe_->send_deque_->push_end(
            (const ZEN_LIB::dequechunk_node *)node);
    }
    if (!ret) {
        ZEN_Trace_LogMsg::debug_errorex(SOCKET_BUS_PUSH_SEND_FAIL, ret);
        return 1;
    }
    return 0;
}

int SocketChannelBus::pop_send_bus(void *&node) {
    bool ret;
    {
        SocketChannelLockGuard guard(bus_pipe_->send_lock_);
        ret = bus_pipe_->send_deque_->pop_front(
            (ZEN_LIB::dequechunk_node *)send_bus_pop_buf_);
    }
    if (!ret) {
        return 1;
    }
    node = send_bus_pop_buf_;
    return 0;
}

int SocketChannelBus::peek_send_bus(size_t &node_size) {
    node_size = 0;
    ZEN_Thread_MT_Bus_Pipe *pipe = bus_pipe_;
    unsigned int value = 0;
    bool is_empty;
    {
        SocketChannelLockGuard guard(pipe->send_lock_);
        is_empty = pipe->send_deque_->empty();
        if (!is_empty) {
            ZEN_LIB::_shm_dequechunk_head *head =
                pipe->send_deque_->dequechunk_head_;
            char *chunk_begin = pipe->send_deque_->dequechunk_database_;
            char *read_pos = chunk_begin + head->deque_begin_;
            char *limit = pipe->send_deque_->dequechunk_database_
                          + pipe->send_deque_->dequechunk_head_->size_of_deque_;
            if (read_pos + 4 <= limit) {
                value = *(unsigned int *)read_pos;
            } else {
                // ORIG 0x000d8a74..0x000d8ab5: the wrap-around path copies
                // only three bytes into the zero-initialised 32-bit value
                // (dst_end is &value + 3), so the high byte stays zero.
                char *src = read_pos;
                char *dst = (char *)&value;
                char *dst_end = dst + 3;
                while (dst != dst_end) {
                    if (src >= limit) {
                        src = pipe->send_deque_->dequechunk_database_;
                    }
                    *dst = *src;
                    ++dst;
                    ++src;
                    if (src >= limit) {
                        src = pipe->send_deque_->dequechunk_database_;
                    }
                }
            }
        }
    }
    if (is_empty) {
        return -1;
    }
    node_size = value;
    return 0;
}
