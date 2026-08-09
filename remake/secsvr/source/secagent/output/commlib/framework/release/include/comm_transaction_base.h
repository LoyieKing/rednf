// Auto-generated header stub from DWARF info
// Original path: output/commlib/framework/release/include/comm_transaction_base.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TRANSACTION_BASE_H_H_
#define SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TRANSACTION_BASE_H_H_

#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "src/commlib/framework/comm_service_info.h"
#include <stddef.h>
#include <time.h>

// sizeof = 104
struct Transaction_Base : public ZEN_Timer_Handler { // line 17
    enum TRANSACTION_RUN_STATE {
        INIT_RUN_STATE = 1,
        RIGHT_RUN_STATE = 2,
        TIMEOUT_RUN_STATE = 3,
        EXCEPTION_RUN_STATE = 5,
    };
    enum TRANSACTION_PROCESS {
        WAIT_PROCESS = 1,
        NEXT_PROCESS = 2,
        EXIT_PROCESS_SUCC = 3,
        EXIT_PROCESS_FAIL = 4,
    };
protected:
static const int INVALID_PHASE_ID;
static const int TRANSACTION_TIME_ID[];
Transaction_Manager *trans_manager_;
unsigned int transaction_id_;
bool trans_locker_;
unsigned int req_qq_uin_;
unsigned int trans_command_;
Transaction_Base::TRANSACTION_RUN_STATE trans_run_state_;
int trans_phase_;
SERVICES_ID req_snd_service_;
SERVICES_ID req_rcv_service_;
SERVICES_ID req_proxy_service_;
unsigned int req_trans_id_;
unsigned int req_session_id_;
unsigned int req_game_app_id_;
unsigned int req_ip_address_;
unsigned int req_frame_option_;
Comm_App_Frame *tmp_recv_frame_;
int trans_timeout_id_;
int trans_touchtimer_id_;
unsigned int wait_cmd_;
time_t trans_create_time_;
ZEN_LOG_PRIORITY trace_log_pri_;
int process_errno_;
public:
Transaction_Base(const Transaction_Base &arg0);
protected:
virtual ~Transaction_Base(); // line 68
public:
Transaction_Base(ZEN_Timer_Queue *arg0, Transaction_Manager *arg1, bool arg2); // line 40
virtual void re_init(); // line 73
virtual void finish(); // line 99
virtual Transaction_Base * create_self(ZEN_Timer_Queue *arg0, Transaction_Manager *arg1); // line 139
virtual Transaction_Base::TRANSACTION_PROCESS on_init(); // line 142
virtual Transaction_Base::TRANSACTION_PROCESS on_active(); // line 145
virtual Transaction_Base::TRANSACTION_PROCESS on_timeout(); // line 161
virtual Transaction_Base::TRANSACTION_PROCESS on_exception(); // line 169
virtual int on_exit(); // line 178
Comm_App_Frame * get_recv_appframe(); // line 390
int get_phase(); // line 395
int get_transaction_id() const; // line 400
unsigned int get_wait_cmd() const; // line 410
unsigned int get_req_ipaddress() const; // line 454
unsigned int get_req_cmd() const; // line 405
Transaction_Manager * get_trans_mgr() const; // line 435
int request_send_buf_to_peer(unsigned int cmd, unsigned int qquin, const SERVICES_ID &rcv_svc, const unsigned char *buf, unsigned int buf_len, unsigned int app_id, unsigned int option); // line 639
int request_send_buf_to_proxy(unsigned int cmd, unsigned int qquin, const SERVICES_ID &proxy_svc, const SERVICES_ID &recv_svc, const unsigned char *buf, unsigned int buf_len, unsigned int app_id, unsigned int option); // line 662
int response_buf_sendback(unsigned int cmd, unsigned int uin, const unsigned char *buf, unsigned int buf_len, unsigned int option); // line 714
int sendbuf_to_service(unsigned int cmd, unsigned int qquin, unsigned int transaction_id, unsigned int backfill_trans_id, const SERVICES_ID &rcv_svc, const SERVICES_ID &proxy_svc, const SERVICES_ID &snd_svc, const unsigned char *buf, size_t buf_len, unsigned int app_id, unsigned int option); // line 688
protected:
int initialize_trans(Comm_App_Frame *proc_frame, unsigned int transid); // line 117
void cancel_touch_timer(); // line 553
void cancel_timeout_timer(); // line 562
int set_timeout_timer(int sec, int usec); // line 473
int set_timetouch_timer(int sec, int usec); // line 504
virtual int handle_timeout(const ZEN_Time_Value &arg0, const void *arg); // line 222
virtual int handle_close(); // line 432
int process_trans_event(); // line 272
int lock_qquin_cmd(); // line 602
void unlock_qquin_cmd(); // line 607
int lock_qquin_key(unsigned int one_key); // line 592
void unlock_qquin_key(unsigned int one_key); // line 597
void set_phase(int arg0); // line 416
void set_state(Transaction_Base::TRANSACTION_RUN_STATE arg0); // line 422
void set_wait_cmd(unsigned int arg0); // line 429
virtual int check_receive_frame(const Comm_App_Frame *recv_frame); // line 572
virtual int receive_trans_msg(Comm_App_Frame *proc_frame); // line 186
int close_request_service() const; // line 535
int check_req_qquin(unsigned int arg0) const; // line 441
int check_request_internal() const; // line 461
void dump_transa_info(ostringstream &strstream) const; // line 615
void output_trans_info(const char *outstr) const; // line 626
};
extern int TRANSACTION_TIME_ID[]; // line 58
extern const int INVALID_PHASE_ID; // line 55
namespace Transaction_Base {
    enum TRANSACTION_RUN_STATE {
        INIT_RUN_STATE = 1,
        RIGHT_RUN_STATE = 2,
        TIMEOUT_RUN_STATE = 3,
        EXCEPTION_RUN_STATE = 5,
    };
    enum TRANSACTION_PROCESS {
        WAIT_PROCESS = 1,
        NEXT_PROCESS = 2,
        EXIT_PROCESS_SUCC = 3,
        EXIT_PROCESS_FAIL = 4,
    };
} // namespace Transaction_Base


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_OUTPUT_COMMLIB_FRAMEWORK_RELEASE_INCLUDE_COMM_TRANSACTION_BASE_H_H_
