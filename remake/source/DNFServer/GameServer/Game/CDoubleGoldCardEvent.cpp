// df_game_r 双倍金币卡事件 CDoubleGoldCardEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写，地址见头文件注释）。
#include "CDoubleGoldCardEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811c30a（弱 W）：仅基类构造 + vptr 置位。
CDoubleGoldCardEvent::CDoubleGoldCardEvent()
{
}

// ORIG 0x0811c326 / 0x0811c356（弱 W）：标准虚析构（空体编译器生成同形）。
CDoubleGoldCardEvent::~CDoubleGoldCardEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c374：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x10,0)
// ("CDoubleGoldCardEvent Event Start!")。
void CDoubleGoldCardEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x10, 0);
    tr("CDoubleGoldCardEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c3c6：thunk——经 vtable slot 2（StartEvent()）虚调用。
void CDoubleGoldCardEvent::StartEvent(Word_Param param)
{
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811c3e0：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x1a,0)
// ("CDoubleGoldCardEvent Event End!")。
void CDoubleGoldCardEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1a, 0);
    tr("CDoubleGoldCardEvent Event End!");
}
