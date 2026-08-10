// Reconstructed from gunnersvr DWARF + disassembly.
// GunnerChannel: monitor_info_(38) + zulu_(80) = 118 -> sizeof 120
// GunnerChannelMgr: default_channel_(120) + channel_num_(4) + gunner_channel_[64] = 7804
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_CHANNEL_MGR_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_CHANNEL_MGR_H_H_

#include "output/protocol/common/release/include/comm_conf_gunnersvr.h"
#include "output/protocol/common/release/include/comm_proto_public_monitorsvr.h"
#include "output/commlib/framework/release/include/comm_sndrcv_zulu.h"

// sizeof = 120
struct GunnerChannel { // line 17
private:
    conf_gunnersvr::MonitorInfo monitor_info_;  // +0 (38)
    Zulu_SendRecv_Package zulu_;                // +40 (80)
public:
    int init(const conf_gunnersvr::MonitorInfo &monitor_info); // line 19
    void close(); // line 21
    int send_to_monitor(sec_proto::MonitorSvrReport pkg); // line 23
    unsigned int get_appid(); // line 25
private:
    int connect_monitor(); // line 27
};

// sizeof = 7804
struct GunnerChannelMgr { // line 34
private:
    static const unsigned int MAX_CHANNEL_NUM = 64; // line 45
    GunnerChannel default_channel_;          // +0 (120)
    unsigned int channel_num_;               // +120
    GunnerChannel gunner_channel_[64];       // +124
public:
    typedef conf_gunnersvr::config *LPCONFIG;
    GunnerChannelMgr(); // line 36
    ~GunnerChannelMgr(); // line 37
    int init_channels(LPCONFIG config); // line 39
    void close_channels(); // line 41
    GunnerChannel * get_channel(unsigned int appid); // line 43
};

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_CHANNEL_MGR_H_H_
