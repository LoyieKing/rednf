// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDimensionActivationEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CDimensionActivationEvent 实现。
// 依据 docs/class_func_reports/CDimensionActivationEvent/*.md + ORIG elf 反汇编。
// 数据面：CDataManager+0x4b18 起 m_dimensionAct（DimensionActivationData）：
//   +0x00 int 日计数、+0x04 map<int,DimensionCoinInfo> m_coinMap。
// <iostream> 引入 _ZStL8__ioinit 动态初始化，键名 _GLOBAL__I__ZN25CDimensionActivationEventC2Ev
// 与 ORIG 0x080eed65 一致（ORIG __static_initialization_and_destruction_0 仅构造 ios_base::Init）。
#include "CDimensionActivationEvent.h"

#include <iostream>
#include <map>
#include "CDataManager.h"
#include "GameTypes.h"   // cMyTrace（定义于 GameUtils.cpp）

#include "CDimensionActivationEvent/CDimensionActivationEvent.inc"
{
    // ORIG 0x080eea84：仅调 CEventBase::CEventBase() 并设置本类 vptr，无成员初始化。
}

#include "CDimensionActivationEvent/dtor_CDimensionActivationEvent.inc"
{
    // ORIG D1 0x080eeaa0：恢复本类 vptr 后调 CEventBase::~CEventBase()；D0 再 operator delete。
}

#include "CDimensionActivationEvent/StartEvent.inc"
{
    // ORIG 0x080eeaee：SetEventFlag(true) → trace → set_dungeon_play_count()。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x12, 0);
    tr("CDimensionActivationEvent Event Start!");
    set_dungeon_play_count();
}

#include "CDimensionActivationEvent/EndEvent.inc"
{
    // ORIG 0x080eeb64：SetEventFlag(false) → trace → reset_dungeon_play_count()。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1e, 0);
    tr("CDimensionActivationEvent Event End!");
    reset_dungeon_play_count();
}

#include "CDimensionActivationEvent/StartEvent_1.inc"
{
    // ORIG 0x080eeb4a：忽略参数，经 vtable+0x08 转调 StartEvent()。
    // [N7 封顶] 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4) 互换。
    (void)param;
    StartEvent();
}

#include "CDimensionActivationEvent/set_dungeon_play_count.inc"
{
    // ORIG 0x080eebc0：读 m_dimensionAct.m_field0（日计数）→ trace；
    // >0 时对难度 0..5 逐个 set_dimensionInout(i, (char)dayCount)。
    int dayCount = G_CDataManager()->m_dimensionAct.m_field0;
    cMyTrace tr(__PRETTY_FUNCTION__, 0x27, 0);
    tr("CDimensionActivationEvent day_play_count : %d", dayCount);
    // [b28c-16] P223：ORIG 尾 `jle +0x90(nop) / then 体尾 jmp +0x91 / +0x90: nop`
    // 空 else 占位岛 ⇒ 复现形=否定 goto + 体尾空标签（4.4 对 if{}else{body}
    // 恒 jump-thread 少 jmp+nop）。
    if (dayCount <= 0)
        goto SKIP;
    for (int i = 0; i < 6; ++i)
        G_CDataManager()->set_dimensionInout(i, (char)dayCount);
SKIP:
    ;
}

#include "CDimensionActivationEvent/reset_dungeon_play_count.inc"
{
    // ORIG 0x080eec58：reset_dimensionInout() → trace。
    G_CDataManager()->reset_dimensionInout();
    cMyTrace tr(__PRETTY_FUNCTION__, 0x34, 0);
    tr("CDimensionActivationEvent reset_dungeon_play_count");
}

// [b28d-23 N·N93] it(-0x10)/e(-0xc) 双迭代器槽方向跨版本相反（4.1 先声明占深槽，4.4 相反），声明序绑死 find/end 求值序，迭代器无默认构造不可延后初始化 ⇒ N。ORIG 0x080eeca4
#include "CDimensionActivationEvent/checkFreeRevivalCondition.inc"
{
    // [B24D判词 N] 语义正确：两次 G_CDataManager（P48 无 CSE）+m_dimensionAct.
    //   m_coinMap find/end（P96）+命中 value 判空。P167 调用点差集 6/6 CLEAR；
    //   diff=lowering。
    // ORIG 0x080eeca4：两次 G_CDataManager()（O0 无 CSE），以 dungeonDiff 在
    // m_dimensionAct.m_coinMap 中 find/end（可变 iterator，P96）；命中且 value
    // 的 +0x04 bool != true → 可免费复活（true），否则 false。分支布局照 ORIG
    // 两个独立条件直跳统一失败出口（P133），bool 返回单出口。
    // [N82/P112 封顶] ORIG 槽序 it=-0x10(深) e=-0xc 与 4.4 相反；e 赋值形引入
    // -0x1c 临时+op= 调用劣化，迭代器无默认构造不可延后初始化 ⇒ 槽互换 4 行封顶。
    // [P6AuditOldN.P6S2 2026-09-11] P339 交换声明序实测反劣：NEAR(38=38,blocks=5)
    //   → DIFF(38=38, 首块即 find/end 调用序对调 + 多 sub $0x4) ⇒ 已回滚。
    //   维持槽互换封顶结论。
    std::map<int, DimensionCoinInfo>::iterator it = G_CDataManager()->m_dimensionAct.m_coinMap.find(dungeonDiff);
    std::map<int, DimensionCoinInfo>::iterator e = G_CDataManager()->m_dimensionAct.m_coinMap.end();
    if (it != e)
    {
        if (it->second.m_field4 != 1)
            return true;
    }
    return false;
}
