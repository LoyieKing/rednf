// ============================================================================
// CPartyQuestLeafC3.cpp —— CParty/CBattle_Field 任务迷宫叶子恢复（2026-08-24，CPartyQuestLeafC3）
//
// 完整读取 docs/class_func_reports/ 对应 .md 的 ## 汇编 + ## 反编译 C 后转写：
//   CBattle_Field::setQuestMaze(bool)                T 0x0830e302
//     （docs/class_func_reports/CBattle_Field/setQuestMaze.md）
//   CParty::getMemberMaxLevel()                      T 0x085b60be
//     （docs/class_func_reports/CParty/getMemberMaxLevel.md；
//      报告内两处调用被 report_resolve 标注为 global-ctor 偏移，经 nm 复核：
//      0x080da2b8 = _ZNK15CUserCharacInfo16get_charac_levelEv（W），
//      0x0808e218 = _ZSt3maxIiERKT_S2_S2_（W）。）
//
// 指派核对项——ORIG 真实符号已由在册 TU 提供，此处跳过定义（防 ODR）：
//   CParty::CheckQuestConnection(int, ENUM_MAZE_QUEST_TYPE) T 0x085b6478
//     → PartyClosureC2.cpp（cmake/game/CMakeLists.txt 已登记）
//   CParty::GetRoutingDiceMemberList(vector<pair<int,int>>&, CItem*) T 0x085ba294
//     → PartyClosureC2.cpp
//   stMapMonsterKillChecker_t::setUseSkillMaterial(unsigned) 全局 W 0x0830e72c
//     → FieldSmallStructs.cpp（已登记）
//
// setUseSkillMaterial 嵌套/全局核对结论（详见 /tmp/df_batches/CPartyQuestLeafC3.md）：
//   ORIG 另有嵌套 W 符号 _ZN8WongWork11CDeathTower25stMapMonsterKillChecker_t19setUseSkillMaterialEj
//   @0x0823464c（仅写 +0x00，无计数自增；嵌套 init @0x0823462a 仅清 0xc 字节）。
//   ORIG DisPatcher_DeleteItem::exercise_mode（T 0x081c1ad6，+0x130 处 call 0x823464c）
//   调用的是该嵌套符号；当前 DisPatcherCore.cpp 将 tower+0xb58 cast 为全局类型后调用
//   全局符号（多出 ++m_field14@+0x14 的写），语义偏差待统一 CDeathTower 尾部布局修正，
//   本 TU 不越权改头/改他文件，仅报告。
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
void CBattle_Field::setQuestMaze(bool flag)
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
int CParty::getMemberMaxLevel()
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
