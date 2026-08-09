// Auto-generated header stub from DWARF info
// Original path: src/commsvr/gunnersvr/gunner_app.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_APP_H_H_
#define SECSVR_SRC_COMMSVR_GUNNERSVR_GUNNER_APP_H_H_

#include "output/protocol/common/release/include/comm_conf_gunnersvr.h"

// sizeof = 552
struct GunnerSvrApp : public Comm_SvrdApp_NonCtrl { // line 22
public:
static const int TICK_TIME;
private:
GunnerChannelMgr::LPCONFIG config_;
static GunnerSvrApp *instance_;
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
