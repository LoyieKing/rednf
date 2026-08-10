// Reconstructed from zergsvr binary DWARF + 反汇编。
// load_app_conf/init/exit 为头文件内联虚函数：二进制中为 WEAK 符号
// （0x8077dd0/0x8077de0/0x8077df0，由 vtable 所在 TU 弱符号化发出）。
#ifndef SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APPLICATION_H_H_
#define SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APPLICATION_H_H_

#include <time.h>

#include "output/commlib/framework/release/include/comm_svrd_application.h"
#include "output/commlib/zenlib/release/include/zen_timer_queue_wheel.h"
#include "output/commlib/framework/release/include/comm_zerg_mmappipe.h"

struct Zerg_Comm_Manager;

// sizeof = 560
struct Zerg_Service_App : public Comm_Svrd_Appliction { // line 17
protected:
static const size_t NORMAL_MAX_ONCE_SEND_FRAME = 4096; // line 24（DWARF const_value）
static const size_t SENDBUSY_MAX_ONCE_SEND_FRAME = 12288; // line 26
static const size_t SEND_BUSY_JUDGE_STANDARD = 2048; // line 29
static const size_t SEND_IDLE_JUDGE_STANDARD = 128; // line 31
static const time_t IDLE_REACTOR_WAIT_USEC = 1000; // line 35
static const time_t NORMAL_REACTOR_WAIT_USEC = 100; // line 37
static const time_t BUSY_REACTOR_WAIT_USEC = 20; // line 39
static const time_t SEND_BUSY_REACTOR_WAIT_USEC = 0; // line 41
static const size_t DEFAULT_IO_FIRST_RATIO = 32; // line 44
public:
ZEN_Timer_Queue *timer_queue_;
Zerg_Comm_Manager *zerg_comm_mgr_;
time_t conf_timestamp_;
protected:
static Zerg_Service_App *instance_;
public:
Zerg_MMAP_BusPipe::ZERG_CONFIG *config_;
Zerg_Service_App(const Zerg_Service_App &arg0);
protected:
Zerg_Service_App(); // line 37
virtual ~Zerg_Service_App(); // line 45
Zerg_Service_App(const Zerg_Service_App *arg0); // line 52
const Zerg_Service_App & operator=(const Zerg_Service_App &arg0); // line 53
public:
virtual int init_instance(); // line 78
virtual int exit_instance(); // line 266
virtual int run_instance(); // line 314
private:
virtual int load_app_conf() { return 0; } // line 81
virtual int init() { return 0; } // line 86
virtual void exit() {} // line 92
virtual int reload(); // line 199
public:
static Zerg_Service_App * instance(); // line 390
static void clean_instance(); // line 402
};
#endif // SECSVR_SRC_COMMSVR_ZERGSVR_ZERG_APPLICATION_H_H_
