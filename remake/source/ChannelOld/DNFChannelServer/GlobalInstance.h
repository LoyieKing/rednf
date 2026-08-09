#ifndef GLOBALINSTANCE_H_
#define GLOBALINSTANCE_H_

#include "stdafx.h"
#include "ThreadLock.h"
#include <assert.h>

class GLOBAL
{
public:
    bool main_thread_stop;
};

template <class T>
class GlobalInstance
{
public:
    static T* m_p;
    static TThreadLock<ThreadLock_linux> sync;

    static T* inst_ptr()
    {
        create();
        return m_p;
    }

    static void create()
    {
        if (m_p == NULL)
        {
            TScopedLock<TThreadLock<ThreadLock_linux> > slock(sync);
            if (m_p == NULL)
            {
                m_p = new T;
            }
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
TThreadLock<ThreadLock_linux> GlobalInstance<T>::sync;

#endif // GLOBALINSTANCE_H_
