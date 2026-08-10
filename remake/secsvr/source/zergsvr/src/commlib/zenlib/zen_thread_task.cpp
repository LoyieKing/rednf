// 还原自 gunnersvr 二进制（zen_thread_task.cpp，GCC 4.1.0 SUSE -O2，2026-08-10）
// Zen_Thread_Task：pthread 线程任务封装。
//   activate 经 ZEN_OS::pthread_createex(svc_run, this, ...) 创建；
//   svc_run 调用虚函数 svc() 并把返回值存入 thread_return_ 后 pthread_exit。
#include "src/commlib/zenlib/zen_thread_task.h"

namespace ZEN_OS {
int pthread_createex(void (*start_routine)(void *), void *arg, unsigned long *threadid,
                     int detachstate, unsigned int stacksize, int threadpriority);
int pthread_join(unsigned long threadid);
int pthread_detach(unsigned long threadid);
void pthread_exit();
} // namespace ZEN_OS

Zen_Thread_Task::Zen_Thread_Task()
    : group_id_(INVALID_GROUP_ID), thread_id_(0), thread_return_(0) {}
Zen_Thread_Task::~Zen_Thread_Task() {}

int Zen_Thread_Task::thread_return() {
    return thread_return_;
}

int Zen_Thread_Task::group_id() const {
    return group_id_;
}

ZEN_THREAD_ID Zen_Thread_Task::thread_id() const {
    return thread_id_;
}

int Zen_Thread_Task::svc() {
    return 0;
}

int Zen_Thread_Task::wait_join() {
    return ZEN_OS::pthread_join(thread_id_);
}

int Zen_Thread_Task::detach() {
    return ZEN_OS::pthread_detach(thread_id_);
}

int Zen_Thread_Task::activate(int group_id, ZEN_THREAD_ID *threadid, int detachstate,
                              size_t stacksize, int threadpriority) {
    int ret = ZEN_OS::pthread_createex(&Zen_Thread_Task::svc_run, this, threadid,
                                       detachstate, stacksize, threadpriority);
    if (ret == 0) {
        group_id_ = group_id;
        thread_id_ = *threadid;
    }
    return ret;
}

void Zen_Thread_Task::svc_run(void *args) {
    Zen_Thread_Task *t = (Zen_Thread_Task *)args;
    t->thread_return_ = t->svc();
    ZEN_OS::pthread_exit();
}
