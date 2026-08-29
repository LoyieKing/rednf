// df_game_r 还原 —— LevelUpBefore70LvEvent（70 级前升级 repeat 事件）。
// 全部函数逐指令对照 docs/class_func_reports/LevelUpBefore70LvEvent/*.md
// 的 ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   C1 0x0811ff60 / D1 0x0811ff7c / D0 0x0811ffac / StartEvent() 0x0811ffca /
//   StartEvent(Word_Param) 0x0812001c / EndEvent() 0x0812006e
#include "LevelUpBefore70LvEvent.h"

// ORIG C1 0x0811ff60：CEventBase::CEventBase() + vptr 设置，无自身字段。
LevelUpBefore70LvEvent::LevelUpBefore70LvEvent()
{
}

// ORIG D1 0x0811ff7c：vptr 设置 + CEventBase::~CEventBase() + 删除标志检查
// （mov $0x0,%eax; test %al,%al; je —— gcc 虚析构标准形态）。
LevelUpBefore70LvEvent::~LevelUpBefore70LvEvent()
{
}

// ORIG 0x0811ffca：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x12,0)。
// 日志名用 __FUNCTION__（ORIG .rodata 符号 _ZZN22...E12__FUNCTION__ 实证）。
void LevelUpBefore70LvEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x12, 0);
    trace("LevelUpBefore70Lv Event start!");
}

// ORIG 0x0812001c：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x18,0)；
// Word_Param 参数未使用（Ghidra C 仅 this 参与）。
void LevelUpBefore70LvEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace trace(__FUNCTION__, 0x18, 0);
    trace("LevelUpBefore70Lv Event start! [monitor request]");
}

// ORIG 0x0812006e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x1e,0)。
// ORIG W 0x0812011c：恒返 1（该事件常驻生效）。
bool LevelUpBefore70LvEvent::IsEventing(CUser* /*user*/) const
{
    return 1;
}

void LevelUpBefore70LvEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__FUNCTION__, 0x1e, 0);
    trace("LevelUpBefore70Lv Event end!");
}
