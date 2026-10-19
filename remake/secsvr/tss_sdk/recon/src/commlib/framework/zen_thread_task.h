// Zen_Thread_Task, shared by the reconstructed commlib/framework headers
// (comm_cfgsvr_sdk.h and comm_socket_channel.h).
//
// ORIG DWARF (libtss_sdk.so.1.0.394):
//   struct Zen_Thread_Task size=16, base ZEN_NON_Copyable @0 (empty-base
//   optimised), vptr @0, group_id_ @4, thread_id_ @8, thread_return_ @12.
//   Emitted symbols: ctor/dtor, svc, svc_run, activate, wait_join, detach,
//   group_id, thread_id, thread_return.
//
// Both framework headers used to declare this class independently; a TU that
// needs CfgSvrSdk/SERVICES_ID *and* SocketChannel (channel_plugin_agent.cpp)
// then failed with a redefinition.  This is the single authoritative
// declaration; every consumer includes it instead of repeating the body.
#ifndef TSS_RECON_ZEN_THREAD_TASK_H_
#define TSS_RECON_ZEN_THREAD_TASK_H_

#include "src/commlib/zenlib/zen_recon_support.h"  // ZEN_NON_Copyable, ZEN_THREAD_ID

class Zen_Thread_Task : public ZEN_NON_Copyable {
public:
    Zen_Thread_Task();
    virtual ~Zen_Thread_Task();
    virtual int svc();

    int activate(int group_id, unsigned long *thread_id, int a,
                 unsigned int stack_size, int b);
    int wait_join();
    int detach();
    int group_id() const;
    unsigned long thread_id() const;
    int thread_return();

protected:
    int group_id_;            // +0x04
    unsigned long thread_id_; // +0x08
    int thread_return_;       // +0x0c
};

typedef char tss_recon_zen_thread_task_size[
    sizeof(Zen_Thread_Task) == 16 ? 1 : -1];

#endif  // TSS_RECON_ZEN_THREAD_TASK_H_
