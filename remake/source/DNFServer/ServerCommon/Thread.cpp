#include "Thread.h"

// 原始二进制静态链入 libpthread 弱符号 pthread_equal（08058610，a==b 比较）。
// 此处以弱定义补齐，避免依赖动态 libpthread 的版本化符号。
extern "C" int pthread_equal(pthread_t t1, pthread_t t2) __attribute__((weak));
extern "C" int pthread_equal(pthread_t t1, pthread_t t2) {
    return t1 == t2;
}

CMutex::CMutex() {
    pthread_mutex_init(&mutex, NULL);
}

CMutex::~CMutex() {
    pthread_mutex_destroy(&mutex);
}

void CMutex::lock() {
    pthread_mutex_lock(&mutex);
}

void CMutex::unlock() {
    pthread_mutex_unlock(&mutex);
}

template<class T>
CGuard<T>::CGuard(T* mutex) {
    // 原始：先 lock（直接对参数调用），再保存成员
    mutex->lock();
    m_mutex = mutex;
}

template<class T>
CGuard<T>::~CGuard() {
    m_mutex->unlock();
}

template class CGuard<CMutex>;
