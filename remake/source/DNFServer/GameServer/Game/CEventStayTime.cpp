#include "CEventStayTime.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "MoreTypes.h"
#include "StreamPool.h"
#include "TimerQueue.h"
#include "TaiwanInternal.h"

TimerQueue* G_TimerQueue();

CEventStayTime::CEventStayTime()
    : CEventBase(), m_rewardStayTime(0), m_intervalStayTime(0),
      m_firstReset(true)
{
}

CEventStayTime::~CEventStayTime()
{
}

void CEventStayTime::StartEvent()
{
    SetEventFlag(true);
    RegistNextTimer();
}

void CEventStayTime::EndEvent()
{
    m_rewardStayTime = 0;
    m_intervalStayTime = 0;
    SetEventFlag(false);
}

void CEventStayTime::StartEvent(Word_Param param)
{
    m_rewardStayTime = static_cast<unsigned int>(param.m_duration) * 60u;
    m_intervalStayTime = static_cast<unsigned int>(param.m_field2) * 60u;
    StartEvent();
}

void CEventStayTime::dailyresetData()
{
    if (!IsEventing(0) || IsFirstReset())
        return;
    if (GlobalData::s_stream_pool == 0 || GlobalData::s_msgq_mgr == 0)
        return;
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Event/EventStayTimeCharacter.cpp", 0x4a);
    if (stream == 0)
        return;
    CStreamGuard guard(stream, true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(7), -1);
    if (guard.GetInBuffer<Taiwan::SigResetStayTimeEvent>() != 0)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CEventStayTime::RegistNextTimer()
{
    if (!IsEventing(0))
        return;
    TimerQueue* queue = G_TimerQueue();
    if (queue != 0)
        queue->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                           static_cast<TIMER_MESSAGE>(0xad),
                           m_intervalStayTime, 0, 0);
}

bool CEventStayTime::IsFirstReset()
{
    if (!m_firstReset)
        return false;
    m_firstReset = false;
    return true;
}

unsigned int CEventStayTime::getRewardStayTime() const
{
    return m_rewardStayTime;
}

unsigned int CEventStayTime::getIntervalStayTime() const
{
    return m_intervalStayTime;
}
