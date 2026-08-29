// df_game_r 进阶祭坛开启事件 CEventAdvanceAltarOpen 实现。
// 报告：docs/class_func_reports/CEventAdvanceAltarOpen/（3 函数全读）。
#include "CEventAdvanceAltarOpen.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08116b2c（弱 W）：调 CEventBase::CEventBase(0x0810ae2c) 后
// `movl $0x8b314e8,(%eax)` 置 vptr（vtable+8），无自有成员初始化。
CEventAdvanceAltarOpen::CEventAdvanceAltarOpen()
{
}

// ORIG 0x080e95e2（弱 W）：D1 `movl $0x8b314e8,(%eax)` 重置 vptr（vtable+8）后
// call CEventBase::~CEventBase(0x0810ae52)，`mov $0x0,%eax; test %al,%al` 判定
// deleting 标志为假；D0 0x080e9612 调 D1 后 operator delete。空体 + 虚析构
// 由编译器生成同形。
CEventAdvanceAltarOpen::~CEventAdvanceAltarOpen()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x080e94dc：空体（仅 push/pop/ret）。
void CEventAdvanceAltarOpen::StartEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x080e94e2：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x48e,0)
// ("CEventAdvanceAltarOpen Event Start! "，尾部带空格)。
void CEventAdvanceAltarOpen::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x48e, 0);
    tr("CEventAdvanceAltarOpen Event Start! ");
}

// ---------------------------------------------------------------------------
// ORIG 0x080e9534：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x494,0)
// ("CEventAdvanceAltarOpen Event End!")。
void CEventAdvanceAltarOpen::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x494, 0);
    tr("CEventAdvanceAltarOpen Event End!");
}
