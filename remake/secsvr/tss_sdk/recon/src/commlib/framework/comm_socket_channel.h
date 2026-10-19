// SocketChannel, reconstructed from libtss_sdk.so.1.0.394, CU
// /data/.../tag394/src/commlib/framework/comm_socket_channel.cpp
//
// ORIG DWARF layout (sizeof 108):
//   +0x00 Zen_Thread_Task (vptr, group_id_, thread_id_, thread_return_) = 16
//   +0x10 volatile bool is_thread_run_
//   +0x14 std::map<int, SocketChannelBusinessInfo*> business_info_map_ (24)
//   +0x2c ZEN_Thread_RW_Mutex business_info_map_rwlock_ (36)
//   +0x50 ZEN_Thread_Mutex    start_thread_lock_ (28)
// Vtable (0x293738): D1, D0, svc.
#ifndef TSS_RECON_COMM_SOCKET_CHANNEL_H_
#define TSS_RECON_COMM_SOCKET_CHANNEL_H_

#include <map>
#include <new>
#include <stddef.h>

#include "src/commlib/framework/comm_socket_channel_bus.h"
#include "src/commlib/framework/comm_socket_channel_info.h"
#include "src/commlib/zenlib/zen_lock_thread_rw_mutex.h"

class ZEN_Time_Value;

// Zen_Thread_Task is shared with comm_cfgsvr_sdk.h; one authoritative
// declaration keeps both headers co-includable (channel_plugin_agent.cpp).
#include "src/commlib/framework/zen_thread_task.h"


class ZEN_Reactor : public ZEN_NON_Copyable {
public:
    ZEN_Reactor();
    virtual ~ZEN_Reactor();
    virtual void close();
    virtual int register_handler(ZEN_Event_Handler *handler, int mask);
    virtual int remove_handler(ZEN_Event_Handler *handler, bool flag);
    virtual int cancel_wakeup(ZEN_Event_Handler *handler, int mask);
    virtual int schedule_wakeup(ZEN_Event_Handler *handler, int mask);
    virtual int handle_events(ZEN_Time_Value *time, unsigned int *ret);

    static ZEN_Reactor *instance();
    static void instance(ZEN_Reactor *reactor);
    static void clean_instance();
};

class ZEN_Epoll_Reactor : public ZEN_Reactor {
public:
    ZEN_Epoll_Reactor(unsigned int max_events, bool flag, int timeout);
    virtual ~ZEN_Epoll_Reactor();
};

class SocketChannel : public Zen_Thread_Task {
public:
    virtual ~SocketChannel();

    int init_connection(SocketChannelInitData &addr);
    int fini();
    int send_data(int business_id, const void *send_buf);
    int recv_data(int *business_id, void *recv_buf, size_t *recv_buf_len);
    int recv_business_data(int business_id, void *recv_buf,
                           size_t *recv_buf_len);
    static SocketChannel *instance();

private:
    SocketChannel();
    SocketChannel(const SocketChannel &arg0);

    int start_thread();
    int stop_thread();
    virtual int svc();
    int send_to_net();
    SocketChannelBusinessInfo *get_business_info(int business_id);

    static int HEAVY_IDLE_INTERVAL_MICROSECOND;
    volatile bool is_thread_run_;                              // +0x10
    std::map<int, SocketChannelBusinessInfo *> business_info_map_; // +0x14
    ZEN_Thread_RW_Mutex business_info_map_rwlock_;             // +0x2c
    ZEN_Thread_Mutex start_thread_lock_;                       // +0x50
    static SocketChannel *instance_;
};

#endif // TSS_RECON_COMM_SOCKET_CHANNEL_H_
