// Reconstructed from gunnersvr DWARF + disassembly (2026-08-10).
// sizeof = 548：基类 Comm_Svrd_Appliction(544) + nonctrl_recv_buffer_(4)。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APP_NON_CTRL_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APP_NON_CTRL_H_H_

#include "src/commlib/framework/comm_svrd_application.h"

struct Comm_App_Frame;

// sizeof = 548
struct Comm_SvrdApp_NonCtrl : public Comm_Svrd_Appliction { // line 12
protected:
    Comm_App_Frame *nonctrl_recv_buffer_; // 0x220
public:
    Comm_SvrdApp_NonCtrl(const Comm_SvrdApp_NonCtrl &arg0);
protected:
    Comm_SvrdApp_NonCtrl(); // line 8
    virtual ~Comm_SvrdApp_NonCtrl(); // line 15
public:
    virtual int run_instance(); // line 26
protected:
    virtual int popfront_recvpipe(size_t &proc_frame); // line 91
    virtual int process_recv_appframe(Comm_App_Frame *arg0) = 0; // line 33
    virtual int proc(size_t &proc_data_num); // line 36（头文件内联，弱符号）
};

inline int Comm_SvrdApp_NonCtrl::proc(size_t &proc_data_num) {
    proc_data_num = 0;
    return 0;
}

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APP_NON_CTRL_H_H_
