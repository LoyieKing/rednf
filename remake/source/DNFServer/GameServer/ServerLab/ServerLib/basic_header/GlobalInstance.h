#ifndef NSL_GLOBALINSTANCE_H_
#define NSL_GLOBALINSTANCE_H_

#include "ThreadLock.h"

namespace nsl {

template <class T>
class GlobalInstance
{
public:
    static T* m_p;
    static TThreadLock<ThreadLock_linux> sync;

    static T* inst_ptr()
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > slock(sync);
        create();
        return m_p;
    }

    static void create()
    {
        if (m_p == NULL)
        {
            if (m_p == NULL)
            {
                m_p = new T;
            }
        }
        else
        {
            return;  // ORIG：外层 if/else + return，产出 jne→nop/jne→出口 形态
        }
    }
};

template <class T>
T* GlobalInstance<T>::m_p = NULL;

template <class T>
TThreadLock<ThreadLock_linux> GlobalInstance<T>::sync;

} // namespace nsl

#endif // NSL_GLOBALINSTANCE_H_
