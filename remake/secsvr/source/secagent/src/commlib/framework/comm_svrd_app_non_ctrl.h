// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_svrd_app_non_ctrl.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APP_NON_CTRL_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APP_NON_CTRL_H_H_

// sizeof = 548
struct Comm_SvrdApp_NonCtrl : public Comm_Svrd_Appliction { // line 12
protected:
Comm_App_Frame *nonctrl_recv_buffer_;
public:
Comm_SvrdApp_NonCtrl(const Comm_SvrdApp_NonCtrl &arg0);
protected:
Comm_SvrdApp_NonCtrl(); // line 8
virtual ~Comm_SvrdApp_NonCtrl(); // line 15
public:
virtual int run_instance(); // line 26
protected:
virtual int popfront_recvpipe(size_t &proc_frame); // line 91
virtual int process_recv_appframe(Comm_App_Frame *arg0); // line 33
virtual int proc(size_t &proc_data_num); // line 36
};


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APP_NON_CTRL_H_H_
