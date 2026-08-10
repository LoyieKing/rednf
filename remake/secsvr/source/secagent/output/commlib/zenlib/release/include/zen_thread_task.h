// 还原头文件（基于 gunnersvr DWARF + 二进制 vtable 验证，2026-08-10）
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_TASK_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_TASK_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include <stddef.h>

// sizeof = 16
struct Zen_Thread_Task : public ZEN_NON_Copyable { // line 36
public:
    static const int INVALID_GROUP_ID = -1;
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

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_TASK_H_H_
