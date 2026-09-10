// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CRestrictCharacCreationEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 限制创建角色事件 CRestrictCharacCreationEvent 实现。
// 报告：docs/class_func_reports/CRestrictCharacCreationEvent/ 全部 7 个方法
// （CRestrictCharacCreationEvent / ~CRestrictCharacCreationEvent /
//  ~CRestrictCharacCreationEvent_08273c9a / StartEvent / StartEvent_08273d0a /
//  EndEvent / getLimitCreateCharacter）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN28CRestrictCharacCreationEventC2Ev`（ORIG 0x8273e2a）；ctor 已是首强符号。
#include <iostream>

#include "CRestrictCharacCreationEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08273c44：基类构造 + vptr 置位 + m_limitType(+0x0c) 置 2。
#include "CRestrictCharacCreationEvent/CRestrictCharacCreationEvent.inc"
{
    m_limitType = 2;
}

// ORIG 0x08273c6a / 0x08273c9a：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CRestrictCharacCreationEvent/dtor_CRestrictCharacCreationEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08273cb8：SetEventFlag(true) +
// cMyTrace("virtual void CRestrictCharacCreationEvent::StartEvent()",0x17,0)
// ("Restrict Character Creation Start!")。
#include "CRestrictCharacCreationEvent/StartEvent.inc"
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x17, 0);
    tr("Restrict Character Creation Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08273d0a：SetEventFlag(true) → m_limitType = param 低 ushort
// （movzwl 0xc(%ebp) → mov %edx,0xc(%eax) 整 4 字节写）→ trace（行 0x23）。
#include "CRestrictCharacCreationEvent/StartEvent_1.inc"
{
    SetEventFlag(true);
    m_limitType = param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x23, 0);
    tr("Restrict Character Creation Start!");
}

// ---------------------------------------------------------------------------
// ORIG 0x08273d68：SetEventFlag(false) +
// cMyTrace("virtual void CRestrictCharacCreationEvent::EndEvent()",0x2b,0)
// ("Restrict Character Creation End!")。
#include "CRestrictCharacCreationEvent/EndEvent.inc"
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x2b, 0);
    tr("Restrict Character Creation End!");
}

// 1 → 限 8；其它 → 0。ORIG 汇编先 test/cmp/jmp 线性分派且 m_limitType 只读
// 一次（mov 0xc(%eax) → test → cmp $1 → jmp else）⇒ switch 决策树形态
// （P46：稀疏 switch 线性 cmp 链，叶块按 case 升序）。
#include "CRestrictCharacCreationEvent/getLimitCreateCharacter.inc"
{
    switch (m_limitType)
    {
    case 0:
        if (characCount == 0)
        {
            return 7;
        }
        break;
    case 1:
        return 8;
    }
    return 0;
}
