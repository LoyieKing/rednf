// df_game_r 清奖励卡事件 CClearRewardCardEvent 实现。
// 报告：docs/class_func_reports/CClearRewardCardEvent/（6 函数全读）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CClearRewardCardEventC2Ev`（ORIG 0x810a873）；ctor 已是首强符号。
#include <iostream>

#include "CClearRewardCardEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0810a720：仅基类构造 + vptr 置位（无自有成员初始化）。
CClearRewardCardEvent::CClearRewardCardEvent()
{
}

// ORIG 0x0810a73c / 0x0810a76c：D1 重置 vptr 后调 CEventBase::~CEventBase（末尾
// __in_chrg 常量折叠不 delete）；D0 调 D1 后 operator delete。空体 + 虚析构由
// 编译器生成同形。
CClearRewardCardEvent::~CClearRewardCardEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0810a78a：空体（仅 push/pop/ret）。
void CClearRewardCardEvent::StartEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0810a790：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x18,0)
// ("Clear reward card event Start")。
void CClearRewardCardEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Clear reward card event Start");
}

// ---------------------------------------------------------------------------
// ORIG 0x0810a7e2：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x1e,0)
// ("Clear reward card event End")。
void CClearRewardCardEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Clear reward card event End");
}
