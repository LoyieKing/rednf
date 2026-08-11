#ifndef GLOBALINSTANCE_H_
#define GLOBALINSTANCE_H_

#include "stdafx.h"
#include <pthread.h>
#include <assert.h>

class GLOBAL
{
public:
    bool main_thread_stop;
};

class Mutex
{
public:
    Mutex();
    ~Mutex();
    void lock();
    void unlock();
    pthread_mutex_t* get_ptr()
    {
        return &sync;
    }

    pthread_mutex_t sync;
};

template <class T>
class GlobalInstance
{
public:
    static T* m_p;
    static Mutex sync;

    static T* inst_ptr()
    {
        create();
        return m_p;
    }

    static void create()
    {
        if (m_p == NULL)
        {
            sync.lock();
            if (m_p == NULL)
            {
                m_p = new T;
            }
            sync.unlock();
#line 52 "GlobalInstance.h"
            assert(m_p);
        }
        else
        {
            return;  // ORIG：外层 if/else + return 出口形态
        }
    }
};

template <class T>
T* GlobalInstance<T>::m_p = NULL;

template <class T>
Mutex GlobalInstance<T>::sync;

#endif // GLOBALINSTANCE_H_
