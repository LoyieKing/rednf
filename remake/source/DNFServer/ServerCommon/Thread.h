#ifndef REDNF_THREAD_H_
#define REDNF_THREAD_H_

#include <pthread.h>

// 原始二进制中的线程工具（Ghidra 反编译：pthread_mutex_* 直接作用于对象自身，偏移 0）
class CMutex {
public:
    CMutex() throw();
    ~CMutex() throw();
    void lock();
    void unlock();
private:
    pthread_mutex_t mutex;  // offset 0
};

template<class T>
class CGuard {
public:
    CGuard(T* mutex);
    ~CGuard();
private:
    T* m_mutex;
};

#endif  // REDNF_THREAD_H_
