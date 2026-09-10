// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPartyQuestLeafC3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>

#include "QuickParty.h"      // 需先于 CParty.h（守卫次序，同 CPartyDepsB2.cpp）
#include "CParty.h"          // cMember m_member[4]/_checkValidUser
#include "CBattle_Field.h"   // m_questMaze（+0x110）
#include "CUserCharacInfo.h" // get_charac_level() const（经 CUser 继承）

// ============================================================================
// CBattle_Field::setQuestMaze —— ORIG T 0x0830e302（setQuestMaze.md）
// 汇编：sub $0x4；mov %al,-0x4(%ebp)；mov 0x8(%ebp),%eax；movzbl -0x4(%ebp),%edx；
//       mov %dl,0x110(%eax)；leave; ret。
// 即形参落栈后按布尔字节写入 this+0x110 == m_questMaze。
// 注：m_questMaze 已在 CBattle_Field.h 修正为 bool（1 字节，布局不变）——
// bool 成员直赋才能复现 ORIG 的「先取 this、后 movzbl 读回」指令次序
// （char 成员会先做 bool→char 扩展，与 ORIG 次序不符，经探针核实）。
// ============================================================================
#include "CPartyQuestLeafC3/setQuestMaze.inc"
{
    m_questMaze = flag;
}

// ============================================================================
// CParty::getMemberMaxLevel —— ORIG T 0x085b60be（getMemberMaxLevel.md）
// 汇编：maxLevel(-0x14)=0；i(-0xc)=0..3 循环：
//   call _checkValidUser(i)；xor $1 / test / jne → 无效槽跳过；
//   eax = *(int*)(this + i*0x18 + 0x78)（== m_member[i].m_pUser）作 this 调
//   CUserCharacInfo::get_charac_level()；std::max<int>(&maxLevel,&level) 回写。
// 返回 maxLevel。转写与同族 getMemberMaxLevelBeforeDungeon（CPartyDepsB2.cpp）一致。
// ============================================================================
#include "CPartyQuestLeafC3/getMemberMaxLevel.inc"
{
    int level;                                          // -0x10（槽位对齐 ORIG）
    int maxLevel = 0;                                   // -0x14
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (!_checkValidUser(i))
            continue;
        level = m_member[i].m_pUser->get_charac_level();
        maxLevel = std::max(maxLevel, level);
    }
    return maxLevel;
}
