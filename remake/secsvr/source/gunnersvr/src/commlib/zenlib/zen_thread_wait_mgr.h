// 还原头文件（基于 gunnersvr DWARF + 二进制验证，2026-08-10）
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_WAIT_MGR_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_WAIT_MGR_H_H_

#include "src/commlib/zenlib/zen_boost_non_copyable.h"
#include "src/commlib/zenlib/zen_os_adapt_predefine.h"
#include "src/commlib/zenlib/zen_thread_task.h"
#include <list>

// sizeof = 8
struct Zen_Thread_Wait_Manager : public ZEN_NON_Copyable { // line 16
    // sizeof = 8
    struct MANAGE_WAIT_INFO {
    public:
        ZEN_THREAD_ID wait_thr_id_;
        int wait_group_id_;
        MANAGE_WAIT_INFO(ZEN_THREAD_ID wait_thr_id, int wait_group_id) // line 24
            : wait_thr_id_(wait_thr_id), wait_group_id_(wait_group_id) {}
        ~MANAGE_WAIT_INFO() {} // line 29
    };
protected:
    static Zen_Thread_Wait_Manager *instance_;
    std::list<MANAGE_WAIT_INFO> wait_thread_list_;
public:
    Zen_Thread_Wait_Manager(); // line 54
    ~Zen_Thread_Wait_Manager(); // line 55
    void record_wait_thread(ZEN_THREAD_ID wait_thr_id, int wait_group_id); // line 58
    void record_wait_thread(const Zen_Thread_Task *wait_thr_task); // line 60
    void wait_all(); // line 63
    void wait_group(int group_id); // line 66
    static Zen_Thread_Wait_Manager * instance(); // line 71
    static void clean_instance(); // line 73
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_THREAD_WAIT_MGR_H_H_
