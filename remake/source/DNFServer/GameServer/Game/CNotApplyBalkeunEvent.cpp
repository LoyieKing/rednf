// df_game_r 不适用巴尔肯事件 CNotApplyBalkeunEvent 实现。
// 报告：docs/class_func_reports/CNotApplyBalkeunEvent/ 全部 6 个方法。
#include "CNotApplyBalkeunEvent.h"

#include "CEnvironment.h"          // G_CEnvironment / m_serverEnvir.m_channelNo
#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081b9d5c：仅基类构造 + vptr 置位。
CNotApplyBalkeunEvent::CNotApplyBalkeunEvent()
{
}

// ORIG 0x081b9d78 / 0x081b9da8：空体 + 虚析构由编译器生成同形。
CNotApplyBalkeunEvent::~CNotApplyBalkeunEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x081b9dc6：IsEventing(0) 为假时置事件标志并打日志（带 server 号）。
void CNotApplyBalkeunEvent::StartEvent()
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x16, 0);
        tr("Not Apply Balkeun Event Start! server(%d)", serverNo);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081b9e4c：同 StartEvent()（忽略 param）。
void CNotApplyBalkeunEvent::StartEvent(Word_Param param)
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
        tr("Not Apply Balkeun Event Start! server(%d)", serverNo);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081b9ed2：IsEventing(0) 为真时清事件标志并打日志。
void CNotApplyBalkeunEvent::EndEvent()
{
    if (IsEventing(0))
    {
        SetEventFlag(false);
        cMyTrace tr(__PRETTY_FUNCTION__, 0x28, 0);
        tr("Not Apply Balkeun Event End");
    }
}
