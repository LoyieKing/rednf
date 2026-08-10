// Reconstructed from gunnersvr DWARF + disassembly (2026-08-10).
// sizeof = 544：ZEN_Server_Toolkit（非多态，488 字节）位于 +4（vptr 占 +0），
// 自身成员自 +0x1ec 起（偏移已用二进制 mov disp(%reg) 核对）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APPLICATION_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APPLICATION_H_H_

#include "output/commlib/zenlib/release/include/zen_server_toolkit.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "src/commlib/framework/comm_service_info.h"
#include <stddef.h>
#include <string>

struct Zerg_MMAP_BusPipe;
struct Comm_Timer_Handler;

// sizeof = 544
struct Comm_Svrd_Appliction : public ZEN_Server_Toolkit { // line 33
protected:
    static const size_t MAX_ONCE_PROCESS_FRAME = 2048; // line 160
    static const unsigned int LIGHT_IDLE_SELECT_INTERVAL = 512; // line 163
    static const unsigned int HEAVY_IDLE_SLEEP_INTERVAL = 10240; // line 166
    static const int LIGHT_IDLE_INTERVAL_MICROSECOND = 10000; // line 170
    static const int HEAVY_IDLE_INTERVAL_MICROSECOND = 100000; // line 171
    static const int CHECK_CONFIG_UPDATE_TIME = 10; // line 174
    static const unsigned int FRAMEWORK_TIMER_INTERVAL = 100000; // line 176

    std::string app_base_name_;                  // 0x1ec
    std::string app_run_name_;                   // 0x1f0
    bool app_run_;                               // 0x1f4
    bool app_pause_;                             // 0x1f5
    SERVICES_ID self_services_id_;               // 0x1f6
    bool run_as_win_serivces_;                   // 0x1fc
    size_t max_msg_num_;                         // 0x200
    std::string app_author_;                     // 0x204
    Zerg_MMAP_BusPipe *zerg_mmap_pipe_;          // 0x208
    std::string service_name_;                   // 0x20c
    std::string service_desc_;                   // 0x210
    int argc_;                                   // 0x214
    const char **argv_;                          // 0x218
private:
    Comm_Timer_Handler *timer_handler_;          // 0x21c
protected:
    static Comm_Svrd_Appliction *base_instance_;
public:
    Comm_Svrd_Appliction(const Comm_Svrd_Appliction &arg0);
protected:
    Comm_Svrd_Appliction(); // line 32
    virtual ~Comm_Svrd_Appliction(); // line 47
public:
    void set_pause_sign(bool app_pause); // line 357
    void set_run_sign(bool app_run); // line 363
    void set_log_priority(ZEN_LOG_PRIORITY log_prio); // line 459
    ZEN_LOG_PRIORITY get_log_priority(); // line 465
    const char * get_app_runname(); // line 471
    const char * get_app_basename(); // line 477
    int run(int argc, const char **argv); // line 53
    int do_run(); // line 858
    const char * get_app_version(); // line 65
    void set_service_info(const char *svc_name, const char *svc_desc); // line 898
    int reload_config(); // line 509
protected:
    virtual int load_app_conf(); // line 261
    virtual int init() = 0; // line 80
    virtual int reload() = 0; // line 84
    virtual void exit(); // line 267
    virtual int init_instance(); // line 96
    int proc_start_args(int argc, const char **argv); // line 804
    virtual int exit_instance(); // line 332
    virtual int run_instance() = 0; // line 100
    virtual int reload_instance(); // line 326
private:
    int register_comm_timer(); // line 483
    int create_app_name(const char *argv_0); // line 272
    int daemon_init(); // line 394
    int process_signal(); // line 368
    int init_log(); // line 912
    static void exit_signal(int arg0); // line 438
    static void reload_config_signal(int arg0); // line 445
public:
    static Comm_Svrd_Appliction * instance(); // line 798
};

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APPLICATION_H_H_
