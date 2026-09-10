// df_game_r 阿拉德红叶事件 Arad_MomijiEvent 实现（Game 域；Monitor/ 同名类为
// 另一独立模块，与此无关）。
// 报告：docs/class_func_reports/Arad_MomijiEvent/ 全部 6 个方法已读。
//   C1 0x0819e738 / D1 0x0819e754 / D0 0x0819e784 / StartEvent 0x0819e7a2 /
//   StartEvent(Word_Param) 0x0819e7f4（thunk）/ EndEvent 0x0819e80e。
// 注意：StartEvent()/EndEvent() 中 trace 在 SetEventFlag 之前（ORIG 汇编序）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit ⇒
// 本 TU 发射 `_GLOBAL__I__ZN16Arad_MomijiEventC2Ev`（ORIG 0x819e89f）；ctor 已是首强符号。
#include <iostream>

#include "Arad_MomijiEvent.h"

#include "GameTypes.h"   // cMyTrace

// ORIG 0x0819e738：仅基类构造 + vptr 置位。
Arad_MomijiEvent::Arad_MomijiEvent()
{
}

// ORIG 0x0819e754 / 0x0819e784：空体 + 虚析构由编译器生成同形。
Arad_MomijiEvent::~Arad_MomijiEvent()
{
}

// ORIG 0x0819e7a2：cMyTrace(PRETTY, 0x14, 0)("[MOMIJI_EVENT] Event Start.")
// 在前，SetEventFlag(true) 在后。
void Arad_MomijiEvent::StartEvent()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x14, 0);
    trace("[MOMIJI_EVENT] Event Start.");
    SetEventFlag(true);
}

// ORIG 0x0819e7f4（thunk，0x1a）：经 vptr+8 虚调 StartEvent()。
void Arad_MomijiEvent::StartEvent(Word_Param /*param*/)
{
    // [b28c-27] N7 封顶：唯一残差=虚调用目标寄存器（ORIG `mov (%eax),%edx; ...;
    // call *%edx` vs 4.4 `mov (%eax),%eax; ...; call *%eax`）；4.4 间接 call 目标
    // 恒 %eax，源码形不可择（探针 /tmp/l22p/vprobe*.cpp）。
    StartEvent();
}

// ORIG 0x0819e80e：cMyTrace(PRETTY, 0x1f, 0)("[MOMIJI_EVENT] Event End.")
// 在前，SetEventFlag(false) 在后。
void Arad_MomijiEvent::EndEvent()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1f, 0);
    trace("[MOMIJI_EVENT] Event End.");
    SetEventFlag(false);
}
