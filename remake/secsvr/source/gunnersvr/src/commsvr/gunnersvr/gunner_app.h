// Reconstructed from gunnersvr DWARF + disassembly.
// sizeof = 552：基类 Comm_SvrdApp_NonCtrl(548) + config_(4)。
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_APP_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_APP_H_H_

#include "output/commlib/framework/release/include/comm_svrd_app_non_ctrl.h"
#include "src/commsvr/gunnersvr/gunner_channel_mgr.h"

// sizeof = 552
struct GunnerSvrApp : public Comm_SvrdApp_NonCtrl { // line 22
public:
    static const int TICK_TIME = 200000; // line 28 (microseconds)
private:
    GunnerChannelMgr::LPCONFIG config_;        // +548
    static GunnerSvrApp *instance_;            // line 23
public:
    GunnerSvrApp(const GunnerSvrApp &arg0);
    GunnerSvrApp(); // line 25
    virtual ~GunnerSvrApp(); // line 30
    virtual int process_recv_appframe(Comm_App_Frame *recv_frame); // line 45
    virtual int proc(size_t &proc_data_num); // line 62
    static GunnerSvrApp * instance(); // line 36
    virtual int reload(); // line 52
private:
    virtual int load_app_conf(); // line 130
    virtual int init(); // line 69
    int register_timer(); // line 101
};

#endif // SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_APP_H_H_
