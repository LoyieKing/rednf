// 还原自 gunnersvr 二进制（zen_thread_wait_mgr.cpp，GCC 4.1.0 SUSE -O2，2026-08-10）
// 单例线程等待管理器：record_wait_thread 登记、wait_all/wait_group 批量 join。
// 注意 wait_group 保留原版 bug-for-bug 语义：匹配节点 join 后 ++iter_temp，
// 然后 erase 的是"递增后的迭代器"（即后一节点），且循环继续使用已释放的迭代器
// （与二进制反汇编逐指令一致，见验证）。
#include "src/commlib/zenlib/zen_thread_wait_mgr.h"

namespace ZEN_OS {
int pthread_join(unsigned long threadid);
} // namespace ZEN_OS

Zen_Thread_Wait_Manager *Zen_Thread_Wait_Manager::instance_ = 0;

Zen_Thread_Wait_Manager::Zen_Thread_Wait_Manager() {}
Zen_Thread_Wait_Manager::~Zen_Thread_Wait_Manager() {}

void Zen_Thread_Wait_Manager::record_wait_thread(ZEN_THREAD_ID wait_thr_id, int wait_group_id) {
    MANAGE_WAIT_INFO wait_thread(wait_thr_id, wait_group_id);
    wait_thread_list_.push_back(wait_thread);
}

void Zen_Thread_Wait_Manager::record_wait_thread(const Zen_Thread_Task *wait_thr_task) {
    MANAGE_WAIT_INFO wait_thread(wait_thr_task->thread_id(), wait_thr_task->group_id());
    wait_thread_list_.push_back(wait_thread);
}

void Zen_Thread_Wait_Manager::wait_all() {
    while (wait_thread_list_.size() > 0) {
        MANAGE_WAIT_INFO wait_thread = wait_thread_list_.front();
        ZEN_OS::pthread_join(wait_thread.wait_thr_id_);
        wait_thread_list_.pop_front();
    }
}

void Zen_Thread_Wait_Manager::wait_group(int group_id) {
    std::list<MANAGE_WAIT_INFO>::iterator iter_temp = wait_thread_list_.begin();
    std::list<MANAGE_WAIT_INFO>::iterator del_iter;
    while (iter_temp != wait_thread_list_.end()) {
        if (iter_temp->wait_group_id_ == group_id) {
            ZEN_OS::pthread_join(iter_temp->wait_thr_id_);
            del_iter = iter_temp;
            ++iter_temp;
            wait_thread_list_.erase(iter_temp); // 原版语义：erase 递增后的迭代器
        } else {
            ++iter_temp;
        }
    }
}

Zen_Thread_Wait_Manager * Zen_Thread_Wait_Manager::instance() {
    if (instance_ == 0) {
        instance_ = new Zen_Thread_Wait_Manager;
    }
    return instance_;
}

void Zen_Thread_Wait_Manager::clean_instance() {
    delete instance_;
    instance_ = 0;
}
