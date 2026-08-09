// Auto-generated header stub from DWARF info
// Original path: src/commlib/framework/comm_svrd_application.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APPLICATION_H_H_
#define SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APPLICATION_H_H_

#include "output/commlib/zenlib/release/include/zen_server_toolkit.h"
#include "output/commlib/zenlib/release/include/zen_trace_log_basic.h"
#include "src/commlib/framework/comm_service_info.h"
#include <bits/stringfwd.h>
#include <stddef.h>

// sizeof = 544
struct Comm_Svrd_Appliction : public ZEN_Server_Toolkit { // line 33
public:
int (***_vptr.Comm_Svrd_Appliction)(...);
protected:
static const size_t MAX_ONCE_PROCESS_FRAME;
static const unsigned int LIGHT_IDLE_SELECT_INTERVAL;
static const unsigned int HEAVY_IDLE_SLEEP_INTERVAL;
static const int LIGHT_IDLE_INTERVAL_MICROSECOND;
static const int HEAVY_IDLE_INTERVAL_MICROSECOND;
static const int CHECK_CONFIG_UPDATE_TIME;
static const unsigned int FRAMEWORK_TIMER_INTERVAL;
string app_base_name_;
string app_run_name_;
bool app_run_;
bool app_pause_;
SERVICES_ID self_services_id_;
bool run_as_win_serivces_;
size_t max_msg_num_;
string app_author_;
Zerg_MMAP_BusPipe *zerg_mmap_pipe_;
string service_name_;
string service_desc_;
int argc_;
const char **argv_;
static Comm_Svrd_Appliction *base_instance_;
private:
Comm_Timer_Handler *timer_handler_;
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
virtual int init(); // line 80
virtual int reload(); // line 84
virtual void exit(); // line 267
virtual int init_instance(); // line 96
int proc_start_args(int argc, const char **argv); // line 804
virtual int exit_instance(); // line 332
virtual int run_instance(); // line 100
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
extern const size_t MAX_ONCE_PROCESS_FRAME; // line 160
extern const unsigned int LIGHT_IDLE_SELECT_INTERVAL; // line 163
extern const unsigned int HEAVY_IDLE_SLEEP_INTERVAL; // line 166
extern const int LIGHT_IDLE_INTERVAL_MICROSECOND; // line 170
extern const int HEAVY_IDLE_INTERVAL_MICROSECOND; // line 171
extern const unsigned int FRAMEWORK_TIMER_INTERVAL; // line 176


extern void __static_initialization_and_destruction_0(int __initialize_p, int __priority); // inferred

#endif // SECSVR_SRC_COMMLIB_FRAMEWORK_COMM_SVRD_APPLICATION_H_H_
