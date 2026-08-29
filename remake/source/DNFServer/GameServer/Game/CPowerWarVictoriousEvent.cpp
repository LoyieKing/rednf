// df_game_r 力战胜利事件 CPowerWarVictoriousEvent 实现。
// 报告：docs/class_func_reports/CPowerWarVictoriousEvent/ 全部 6 个方法
// （CPowerWarVictoriousEvent / ~CPowerWarVictoriousEvent / ~CPowerWarVictoriousEvent_08268968 /
//  StartEvent / StartEvent_08268a68 / EndEvent）。
#include "CPowerWarVictoriousEvent.h"

#include "CEnvironment.h"          // G_CEnvironment / m_serverEnvir.m_channelNo
#include "CDataManager.h"          // G_CDataManager / m_powerParameter.m_victoriousRewardTime
#include "TimerQueue.h"            // G_TimerQueue / TimerEntry / TIMER_MESSAGE
#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0826891c：仅基类构造 + vptr 置位（无自有成员初始化）。
CPowerWarVictoriousEvent::CPowerWarVictoriousEvent()
{
}

// ORIG 0x08268938 / 0x08268968：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CPowerWarVictoriousEvent::~CPowerWarVictoriousEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08268986：IsEventing(0) 为假时置事件标志、挂 0x8b 定时器
// （时长 = m_victoriousRewardTime × 0x3c 分钟）并打日志。
void CPowerWarVictoriousEvent::StartEvent()
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        int rewardTime = G_CDataManager()->m_powerParameter.m_victoriousRewardTime * 0x3c;
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8b,
                                    rewardTime, 0, 0);
        register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x22, 0);
        tr("Power War Victorious Event Start! server(%d)", serverNo);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x08268a68：同 StartEvent()（ORIG 未写 m_param）。
void CPowerWarVictoriousEvent::StartEvent(Word_Param param)
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        int rewardTime = G_CDataManager()->m_powerParameter.m_victoriousRewardTime * 0x3c;
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8b,
                                    rewardTime, 0, 0);
        register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x34, 0);
        tr("Power War Victorious Event Start! server(%d)", serverNo);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x08268b4a：IsEventing(0) 为真时清事件标志、清零 m_param（movw×2）并打日志。
void CPowerWarVictoriousEvent::EndEvent()
{
    if (IsEventing(0))
    {
        SetEventFlag(false);
        m_param.m_duration = 0;
        m_param.m_field2 = 0;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x41, 0);
        tr("Power War Victorious Event End");
    }
}
