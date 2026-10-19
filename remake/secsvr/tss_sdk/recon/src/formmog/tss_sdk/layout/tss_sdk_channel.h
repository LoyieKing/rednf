// Canonical layout of tss_sdk_impl::CIpcChannel and
// tss_sdk_impl::TssSdkChannelPlugin.
//
// ORIG evidence:
//   python3 audit/tu_report.py --type 'tss_sdk_impl::CIpcChannel'
//     struct tss_sdk_impl::CIpcChannel size=75856 bases=0 vfuncs=0
//       +0     recv_buf_      char[65535]        (static const RECV_BUF_SIZE)
//       +65536 thread_mutex_  ZEN_Thread_Mutex   (28; 65535 -> 65536 padding)
//       +65564 bus_channel_   ZEN_BusPipe_TwoWay (10292)
//   python3 audit/tu_report.py --type 'tss_sdk_impl::TssSdkChannelPlugin'
//     struct tss_sdk_impl::TssSdkChannelPlugin size=75908 bases=1
//       base +0     tss_sdk_impl::ITssSdkPlugin          (16)
//       +16     svr_id_pool_mgr_          BusChannelIdMgr (24)
//       +40     channel_id_               unsigned int
//       +44     m_is_use_socket_channel_  bool
//       +45     m_is_use_ipc_channel_     bool
//       +48     m_ipc_channel_            CIpcChannel    (75856)
//       +75904  recv_buf_                 unsigned char*
//     ORIG vtable _ZTVN12tss_sdk_impl19TssSdkChannelPluginE is the
//     ITssSdkPlugin slot set plus the two derived destructor entries
//     (D1 at 0x000a0960, D0 at 0x000a0a00); no virtual is introduced by the
//     derived class beyond ~TssSdkChannelPlugin.  CIpcChannel is
//     non-polymorphic (no vptr: recv_buf_ is at +0).
//
// The declarations below are exactly the ones the reconstruction already uses,
// so every mangled name is unchanged, e.g.
//   _ZN12tss_sdk_impl11CIpcChannel4openEjj
//   _ZN12tss_sdk_impl11CIpcChannel4recvEPPvPj
//   _ZN12tss_sdk_impl11CIpcChannel4sendEPKvj
//   _ZN12tss_sdk_impl11CIpcChannelC2Ev
//   _ZN12tss_sdk_impl19TssSdkChannelPlugin4initEPv
//   _ZN12tss_sdk_impl19TssSdkChannelPlugin4procEPv
//   _ZN12tss_sdk_impl19TssSdkChannelPlugin6notifyEPv
//   _ZN12tss_sdk_impl19TssSdkChannelPlugin23send_pkg_to_ipc_channelERK32TssSdkPluginChannelIpcChannelPkg
//   _ZN12tss_sdk_impl19TssSdkChannelPlugin24send_pkg_to_game_channelERK33TssSdkPluginChannelGameChannelPkg
//   _ZN12tss_sdk_impl19TssSdkChannelPlugin16ipc_channel_recvEPv
//
// TODO(migrate): channel_plugin_agent.cpp and channel_misc_agent.cpp still
// declare their own CIpcChannel / TssSdkChannelPlugin / BusChannelIdMgr /
// ZEN_Thread_Mutex / ZEN_BusPipe_TwoWay models; they must drop those bodies and
// include this header plus layout/zen_bus_pipe.h.  Concretely:
//   channel_plugin_agent.cpp: class ZEN_Thread_Mutex (line 70),
//     class ZEN_BusPipe_TwoWay (line 85), class CIpcChannel (line 101),
//     class BusChannelIdMgr (line 119), class TssSdkChannelPlugin (line 441)
//   channel_misc_agent.cpp:   class ZEN_Thread_Mutex (line 76),
//     struct ZEN_BusPipe_TwoWay (line 99), CIpcChannel body (line 286)
// The CIpcChannel destructor is defined inline in the current models; the
// canonical form declares it and the migrating TU defines it out of line
// (CIpcChannel::~CIpcChannel() {}) - the same object code, no new symbol was
// present in ORIG.
#ifndef TSS_SDK_LAYOUT_TSS_SDK_CHANNEL_H_
#define TSS_SDK_LAYOUT_TSS_SDK_CHANNEL_H_

#include "src/appcomm/app_bus_channel_id_mgr.h"
#include "src/commlib/zenlib/zen_lock_thread_mutex.h"
#include "src/formmog/tss_sdk/framework/tss_sdk_plugin_mgr.h"
#include "src/formmog/tss_sdk/layout/layout_contract.h"
#include "src/formmog/tss_sdk/layout/tss_sdk_pkgs.h"
#include "src/formmog/tss_sdk/layout/zen_bus_pipe.h"

namespace tss_sdk_impl {

// 75856 bytes; channel_plugin.h.  Not polymorphic.
class CIpcChannel {
public:
    CIpcChannel();
    ~CIpcChannel();

    int open(unsigned int channel_id, unsigned int ipc_bus_size);
    int recv(void **recv_buf, unsigned int *recv_buf_size);
    int send(const void *send_buf, unsigned int send_buf_size);
    int close();

private:
    char recv_buf_[65535];        // +0x00000
    ZEN_Thread_Mutex thread_mutex_;   // +0x10000
    ZEN_BusPipe_TwoWay bus_channel_;  // +0x1001c

    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(CIpcChannel, recv_buf_, 0,
                                cipcchannel_recv_buf_offset);
        TSS_SDK_OFFSET_CONTRACT(CIpcChannel, thread_mutex_, 65536,
                                cipcchannel_thread_mutex_offset);
        TSS_SDK_OFFSET_CONTRACT(CIpcChannel, bus_channel_, 65564,
                                cipcchannel_bus_channel_offset);
    }
};

// 75908 bytes; channel_plugin.h.
class TssSdkChannelPlugin : public ITssSdkPlugin {
public:
    TssSdkChannelPlugin();
    virtual ~TssSdkChannelPlugin();

    virtual int init(void *init_data);
    virtual int fini();
    virtual int proc(void *data);
    virtual int notify(void *data);

private:
    int socket_channel_recv(void *proc_data);
    int ipc_channel_recv(void *proc_data);
    int send_pkg_to_ipc_channel(
        const TssSdkPluginChannelIpcChannelPkg &ipc_channel_pkg);
    int send_pkg_to_socket_channel(
        const TssSdkPluginChannelSocketChannelPkg &socket_channel_pkg);
    int send_pkg_to_game_channel(
        const TssSdkPluginChannelGameChannelPkg &game_channel_pkg);

    BusChannelIdMgr svr_id_pool_mgr_;   // +0x10
    unsigned int channel_id_;           // +0x28
    bool m_is_use_socket_channel_;      // +0x2c
    bool m_is_use_ipc_channel_;         // +0x2d
    CIpcChannel m_ipc_channel_;         // +0x30
    unsigned char *recv_buf_;           // +0x12880

    static void tss_sdk_layout_contract_() {
        TSS_SDK_OFFSET_CONTRACT(TssSdkChannelPlugin, svr_id_pool_mgr_, 16,
                                tsssdkchannelplugin_svr_id_pool_mgr_offset);
        TSS_SDK_OFFSET_CONTRACT(TssSdkChannelPlugin, channel_id_, 40,
                                tsssdkchannelplugin_channel_id_offset);
        TSS_SDK_OFFSET_CONTRACT(TssSdkChannelPlugin, m_is_use_socket_channel_, 44,
                                tsssdkchannelplugin_use_socket_offset);
        TSS_SDK_OFFSET_CONTRACT(TssSdkChannelPlugin, m_is_use_ipc_channel_, 45,
                                tsssdkchannelplugin_use_ipc_offset);
        TSS_SDK_OFFSET_CONTRACT(TssSdkChannelPlugin, m_ipc_channel_, 48,
                                tsssdkchannelplugin_ipc_channel_offset);
        TSS_SDK_OFFSET_CONTRACT(TssSdkChannelPlugin, recv_buf_, 75904,
                                tsssdkchannelplugin_recv_buf_offset);
    }
};

TSS_SDK_STATIC_ASSERT(sizeof(CIpcChannel) == 75856,
                      cipcchannel_size_must_be_75856);
TSS_SDK_STATIC_ASSERT(sizeof(TssSdkChannelPlugin) == 75908,
                      tsssdkchannelplugin_size_must_be_75908);

}  // namespace tss_sdk_impl

#endif  // TSS_SDK_LAYOUT_TSS_SDK_CHANNEL_H_
