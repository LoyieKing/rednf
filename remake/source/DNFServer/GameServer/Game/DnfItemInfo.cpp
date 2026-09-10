// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DnfItemInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r DnfItemInfo 实现（OrigDataClears 批，2026-08-28）。
// ORIG：ctor W 0x84e910a / reset W 0x84e9094（objdump 逐指令转写）。
// 语义与 Guild/DNFGuild.cpp 的 Guild 域实现一致（同 mangled 名，Game 服务补齐）。
#include "DnfItemInfo.h"

#include "Inven_Item.h"   // ChronicleRandomOptionInfo（本文件唯一定义点）

// ORIG ctor 0x84e910a：UpgradeSeparateInfo C1@+0x2b（0x80cb332）、
// ReservedCapacity C1@+0x2c（0x80cb36c）由编译器成员构造自动生成，函数体仅调 reset()。
#include "DnfItemInfo/DnfItemInfo.inc"
{
    reset();
}

// ORIG reset 0x84e9094：8 个标量字段逐条清零（movb/movl/movw 宽度照 ORIG），
// 随后 RandomOption::reset@+0x1d（0x80cb2d8）、UpgradeSeparateInfo::reset@+0x2b
// （0x80cb346）、ReservedCapacity::reset@+0x2c（0x80cb380）。
#include "DnfItemInfo/reset.inc"
{
    m_seal = 0;           // movb $0,(%eax)
    m_itemId = 0;         // movl $0,0x1(%eax)
    m_attr = 0;           // movb $0,0x5(%eax)
    m_addInfo = 0;        // movl $0,0x6(%eax)
    m_endurance = 0;      // movw $0,0xa(%eax)
    m_extendInfo = 0;     // movl $0,0xc(%eax)
    m_abilityType = 0;    // movb $0,0x10(%eax)
    m_abilityValue = 0;   // movw $0,0x11(%eax)
    m_opt.reset();        // call RandomOption::reset
    m_up.reset();         // call UpgradeSeparateInfo::reset
    m_res.reset();        // call ReservedCapacity::reset
}

// ORIG 0x822abea W：委托两 STEnchantedSkillOption::reset + m_field2 清零 +
// RandomOption::reset。定义置于本 TU（callee reset 均定义于 Inven_Item.cpp）：
// caller TU 不含 callee 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing 帧，
// 保持 ORIG 的 sub $0x18（NL4 实证）。
#include "DnfItemInfo/reset_1.inc"
{
    m_skill0.reset();
    m_skill1.reset();
    m_field2 = 0;
    m_random.reset();
}

// RandomOptionSeed::reset（ORIG W 0x080cb2cc，m_field=0）：定义原在
// Inven_Item.cpp，迁入本 TU（RandomOption::reset 的 callee 之一）——
// RandomOption::reset 所在 TU 不再含全部 callee 定义体 ⇒ 4.4 unit-at-a-time
// 不塌缩 outgoing 帧，保持 ORIG sub $0x18（NL4/NL3 unit-at-a-time 实证）。
#include "DnfItemInfo/reset_2.inc"
{
    m_field = 0;
}
