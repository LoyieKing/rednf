// df_game_r 金卡空白物品事件 CGoldCardBlankItemEvent 实现。
// 报告：docs/class_func_reports/CGoldCardBlankItemEvent/ 全部 6 个方法
// （CGoldCardBlankItemEvent / ~CGoldCardBlankItemEvent /
//  ~CGoldCardBlankItemEvent_0811ce70 / StartEvent / StartEvent_0811ce94 /
//  EndEvent）。
#include "CGoldCardBlankItemEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811ce24：仅基类构造 + vptr 置位（无自有成员）。
CGoldCardBlankItemEvent::CGoldCardBlankItemEvent()
{
}

// ORIG 0x0811ce40 / 0x0811ce70：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CGoldCardBlankItemEvent::~CGoldCardBlankItemEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811ce8e：空体（仅 push/mov/pop/ret）。
void CGoldCardBlankItemEvent::StartEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811ce94：SetEventFlag(true) +
// cMyTrace(__FUNCTION__,0x18,0)("Gold Card Blank Item Event Start")。
void CGoldCardBlankItemEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Gold Card Blank Item Event Start");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811cee6：SetEventFlag(false) +
// cMyTrace(__FUNCTION__,0x1e,0)("Gold Card Blank Item Event End")。
void CGoldCardBlankItemEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Gold Card Blank Item Event End");
}
