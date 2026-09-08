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
        // [b28c-L15 P227] 原 `register int serverNo` 裸 register 占位系为把 channelNo
        //   提前到 ctor 前并落 %ebx（ORIG 0x081b9de0 `mov 0x1b0(%eax),%ebx`）；
        //   全表达式临时链形令 4.4 自然以 %ebx 跨 ctor 保活 ⇒ 删除 register 关键字。
        cMyTrace(__PRETTY_FUNCTION__, 0x16, 0)(
            "Not Apply Balkeun Event Start! server(%d)",
            G_CEnvironment()->m_serverEnvir.m_channelNo);
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x081b9e4c：同 StartEvent()（忽略 param）。
void CNotApplyBalkeunEvent::StartEvent(Word_Param param)
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        // [b28c-L15 P227] 同 StartEvent()（ORIG 0x081b9e66），忽略 param。
        cMyTrace(__PRETTY_FUNCTION__, 0x1f, 0)(
            "Not Apply Balkeun Event Start! server(%d)",
            G_CEnvironment()->m_serverEnvir.m_channelNo);
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
