// Auto-generated header stub from DWARF info
// Original path: output/commsvr/zergsvr/release/include/zerg_application.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_APPLICATION_H_H_
#define SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_APPLICATION_H_H_

#include <time.h>

// sizeof = 560
struct Zerg_Service_App : public Comm_Svrd_Appliction { // line 17
protected:
static const size_t NORMAL_MAX_ONCE_SEND_FRAME;
static const size_t SENDBUSY_MAX_ONCE_SEND_FRAME;
static const size_t SEND_BUSY_JUDGE_STANDARD;
static const size_t SEND_IDLE_JUDGE_STANDARD;
static const time_t IDLE_REACTOR_WAIT_USEC;
static const time_t NORMAL_REACTOR_WAIT_USEC;
static const time_t BUSY_REACTOR_WAIT_USEC;
static const time_t SEND_BUSY_REACTOR_WAIT_USEC;
static const size_t DEFAULT_IO_FIRST_RATIO;
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
virtual int load_app_conf(); // line 81
virtual int init(); // line 86
virtual void exit(); // line 92
virtual int reload(); // line 199
public:
static Zerg_Service_App * instance(); // line 390
static void clean_instance(); // line 402
};
extern const size_t NORMAL_MAX_ONCE_SEND_FRAME; // line 24
extern const size_t SENDBUSY_MAX_ONCE_SEND_FRAME; // line 26
extern const size_t SEND_BUSY_JUDGE_STANDARD; // line 29
extern const size_t SEND_IDLE_JUDGE_STANDARD; // line 31
extern const time_t IDLE_REACTOR_WAIT_USEC; // line 35
extern const time_t NORMAL_REACTOR_WAIT_USEC; // line 37
extern const time_t BUSY_REACTOR_WAIT_USEC; // line 39
extern const time_t SEND_BUSY_REACTOR_WAIT_USEC; // line 41
extern const size_t DEFAULT_IO_FIRST_RATIO; // line 44

#endif // SECSVR_OUTPUT_COMMSVR_ZERGSVR_RELEASE_INCLUDE_ZERG_APPLICATION_H_H_
