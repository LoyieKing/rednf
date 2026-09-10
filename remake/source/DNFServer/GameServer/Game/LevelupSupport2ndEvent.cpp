// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LevelupSupport2ndEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 升级支持二期事件 LevelupSupport2ndEvent 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写；地址见头文件注释）：
//   C1 0x0811b988 / D1 0x0811b9a4 / D0 0x0811b9d4 /
//   StartEvent() 0x0811b9f2 / StartEvent(Word_Param) 0x0811ba44 /
//   EndEvent 0x0811ba5e（均弱符号 W，独立 TU 定义）。
#include "LevelupSupport2ndEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0811b988：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "LevelupSupport2ndEvent/LevelupSupport2ndEvent.inc"
{
}

// ORIG 0x0811b9a4 / 0x0811b9d4：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "LevelupSupport2ndEvent/dtor_LevelupSupport2ndEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x0811b9f2：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x6b,0)
// ("LevelupSupport2ndEvent Event Start!")。
#include "LevelupSupport2ndEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x6b, 0);
    tr("LevelupSupport2ndEvent Event Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x0811ba44：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-30] N7 封顶：同 CPCRoomWorldDropEvent::StartEvent(Word_Param)——
// ORIG 间接 call 目标 %edx vs 4.4 恒 %eax，助记符全等仅寄存器名差。
#include "LevelupSupport2ndEvent/StartEvent_1.inc"
{
    StartEvent();
}

// ---------------------------------------------------------------------------
// ORIG 0x0811ba5e：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x75,0)
// ("LevelupSupport2ndEvent Event End!")。
#include "LevelupSupport2ndEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x75, 0);
    tr("LevelupSupport2ndEvent Event End!");
}
