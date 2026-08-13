#ifndef NSL_TE_ENTITY_H_
#define NSL_TE_ENTITY_H_

#include "TimerThread.h"
#include "InternalMsg.h"

namespace nsl {

template <class T>
class TE_Entity : public ITimeEntity
{
public:
    typedef unsigned long (T::*TimeFunc)(InternalMsg*);

    TE_Entity()
    {
        m_fpt = NULL;
        m_pt2Object = NULL;
        mMsgType = 2;
    }
    virtual ~TE_Entity()
    {
    }
    virtual int operator()()
    {
        return (m_pt2Object->*m_fpt)(pmMsg);
    }
    void regist(int proc_id, unsigned int check_period, unsigned int proc_count,
                void* pTimeHandler, TimeFunc fpt)
    {
        this->proc_id = proc_id;
        this->check_period = check_period;
        this->proc_count = proc_count;
        bActMsg = true;
        this->m_pt2Object = (T*)pTimeHandler;
        this->m_fpt = fpt;
        bWillDelete = true;
        entNo = (unsigned int)this;
        accumulated_tick = 0;
    }

    TimeFunc m_fpt;
    T* m_pt2Object;
};

} // namespace nsl

#endif // NSL_TE_ENTITY_H_
