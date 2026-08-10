// Reconstructed from secagent binary (DWARF layout + vtable + disassembly, 2026-08-10)
// Transaction_Base: sizeof=104, vtable 槽位已按二进制 _ZTV16Transaction_Base 核对：
//   [0/1]~ [2]handle_timeout [3]timer_close(基类) [4]re_init(=0，.cpp 提供限定实现)
//   [5]finish [6]create_self(=0) [7]on_init(=0) [8]on_active(=0) [9]on_timeout
//   [10]on_exception [11]on_exit [12]handle_close [13]check_receive_frame
//   [14]receive_trans_msg
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TRANSACTION_BASE_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TRANSACTION_BASE_H_H_

#include "output/commlib/zenlib/release/include/zen_timer_handler_base.h"
#include "src/commlib/framework/comm_service_info.h"
#include <sstream>
#include <stddef.h>
#include <time.h>

struct Comm_App_Frame;
class Transaction_Manager;

// sizeof = 104
struct Transaction_Base : public ZEN_Timer_Handler { // line 17
    friend class Transaction_Manager;
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
    Transaction_Manager *trans_manager_;   // 0x08
    unsigned int transaction_id_;          // 0x0c
    bool trans_locker_;                    // 0x10
    unsigned int req_qq_uin_;              // 0x14
    unsigned int trans_command_;           // 0x18
    Transaction_Base::TRANSACTION_RUN_STATE trans_run_state_; // 0x1c
    int trans_phase_;                      // 0x20
    SERVICES_ID req_snd_service_;          // 0x24
    SERVICES_ID req_rcv_service_;          // 0x2a
    SERVICES_ID req_proxy_service_;        // 0x30
    unsigned int req_trans_id_;            // 0x38
    unsigned int req_session_id_;          // 0x3c
    unsigned int req_game_app_id_;         // 0x40
    unsigned int req_ip_address_;          // 0x44
    unsigned int req_frame_option_;        // 0x48
    Comm_App_Frame *tmp_recv_frame_;       // 0x4c
    int trans_timeout_id_;                 // 0x50
    int trans_touchtimer_id_;              // 0x54
    unsigned int wait_cmd_;                // 0x58
    time_t trans_create_time_;             // 0x5c
    ZEN_LOG_PRIORITY trace_log_pri_;       // 0x60
    int process_errno_;                    // 0x64
public:
    Transaction_Base(const Transaction_Base &arg0);
protected:
    virtual ~Transaction_Base(); // line 68
public:
    Transaction_Base(ZEN_Timer_Queue *arg0, Transaction_Manager *arg1, bool arg2); // line 40
    // 注意：以下虚函数声明顺序必须与二进制 vtable 槽位一致。
    virtual int handle_timeout(const ZEN_Time_Value &arg0, const void *arg1); // line 222 (vtable[2])
    // vtable[4]：纯虚，派生类必须实现；基类在 comm_transaction_base.cpp 提供限定实现
    virtual void re_init() = 0; // line 73 (vtable[4])
    virtual void finish(); // line 99 (vtable[5])
    virtual Transaction_Base * create_self(ZEN_Timer_Queue *arg0, Transaction_Manager *arg1) = 0; // line 139 (vtable[6])
    virtual Transaction_Base::TRANSACTION_PROCESS on_init() = 0; // line 142 (vtable[7])
    virtual Transaction_Base::TRANSACTION_PROCESS on_active() = 0; // line 145 (vtable[8])
    virtual Transaction_Base::TRANSACTION_PROCESS on_timeout(); // line 161 (vtable[9])
    virtual Transaction_Base::TRANSACTION_PROCESS on_exception(); // line 169 (vtable[10])
    virtual int on_exit(); // line 178 (vtable[11])
    virtual int handle_close(); // line 432 (vtable[12])
    virtual int check_receive_frame(const Comm_App_Frame *recv_frame); // line 572 (vtable[13])
    virtual int receive_trans_msg(Comm_App_Frame *proc_frame); // line 186 (vtable[14])
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
    int process_trans_event(); // line 272
    int lock_qquin_cmd(); // line 602
    void unlock_qquin_cmd(); // line 607
    int lock_qquin_key(unsigned int one_key); // line 592
    void unlock_qquin_key(unsigned int one_key); // line 597
    void set_phase(int arg0); // line 416
    void set_state(Transaction_Base::TRANSACTION_RUN_STATE arg0); // line 422
    void set_wait_cmd(unsigned int arg0); // line 429
    int close_request_service() const; // line 535
    int check_req_qquin(unsigned int arg0) const; // line 441
    int check_request_internal() const; // line 461
    void dump_transa_info(ostringstream &strstream) const; // line 615
    void output_trans_info(const char *outstr) const; // line 626
};
extern int TRANSACTION_TIME_ID[]; // line 58
extern const int INVALID_PHASE_ID; // line 55

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_TRANSACTION_BASE_H_H_
