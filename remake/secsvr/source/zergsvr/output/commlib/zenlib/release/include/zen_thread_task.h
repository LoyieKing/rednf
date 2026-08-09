// Auto-generated header stub from DWARF info
// Original path: output/commlib/zenlib/release/include/zen_thread_task.h
// 内容为类型信息与声明（函数体暂未还原）。
#ifndef SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_TASK_H_H_
#define SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_TASK_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include <stddef.h>

// sizeof = 16
struct Zen_Thread_Task : public ZEN_NON_Copyable { // line 15
public:
int (***_vptr.Zen_Thread_Task)(...);
static const int INVALID_GROUP_ID;
protected:
int group_id_;
ZEN_THREAD_ID thread_id_;
int thread_return_;
public:
Zen_Thread_Task(const Zen_Thread_Task &arg0);
Zen_Thread_Task(); // line 9
virtual ~Zen_Thread_Task(); // line 16
int activate(int group_id, ZEN_THREAD_ID *threadid, int detachstate, size_t stacksize, int threadpriority); // line 44
int thread_return(); // line 67
int group_id() const; // line 73
ZEN_THREAD_ID thread_id() const; // line 78
int detach(); // line 84
int wait_join(); // line 90
protected:
virtual int svc(); // line 97
static void svc_run(void *args); // line 23
};
extern const int INVALID_GROUP_ID; // line 20

#endif // SECSVR_OUTPUT_COMMLIB_ZENLIB_RELEASE_INCLUDE_ZEN_THREAD_TASK_H_H_
