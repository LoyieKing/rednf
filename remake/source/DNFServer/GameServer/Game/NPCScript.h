#ifndef GAME_NPCSCRIPT_H
#define GAME_NPCSCRIPT_H

// ============================================================================
// CNPCScript / ENUM_NPC_ROLE
//
// 依据 ORIG 报告还原：
//   isKeyItem / isRewardLevel / getPeriodRewardType / load（CNPCScript/ 报告完整）
//   isExistRole（ORIG 0x86d2736 objdump：this+0x50 的 std::map<ENUM_NPC_ROLE,int>
//                上 find(role)!=end()）
//
// ABI 约束：isExistRole 符号 _ZNK10CNPCScript11isExistRoleE13ENUM_NPC_ROLE
//   （枚举标签恰 13 字符）由 Area.cpp 以 asm extern "C" 引用，须保留。
//   其余 ORIG getter（nm -C df_game_r 确认）：
//     _ZN10CNPCScriptC1Ev / D1Ev          CNPCScript() / ~CNPCScript()
//     _ZN10CNPCScript4loadEPKc             load(char const*)
//     _ZNK10CNPCScript13isRewardLevelEt   isRewardLevel(unsigned short) const
//     _ZNK10CNPCScript9isKeyItemEmj        isKeyItem(unsigned long, unsigned) const
//     _ZNK10CNPCScript19getPeriodRewardTypeEv  getPeriodRewardType() const
//
// 布局说明：ORIG CNPCScript 在 +0x04 内嵌一个 NpcScript 对象（权威布局见
// NpcScript.h）。本类按该真实子对象建模；访问器读取内嵌成员的 ORIG 偏移：
//   角色表 NpcScript+0x4c（=CNPCScript+0x50）、key item 表 +0x18c（=+0x190）、
//   奖励等级表 +0x654（=+0x658）、周期奖励名 +0x670（=+0x674）。
// ============================================================================

#include "NpcScript.h"

enum ENUM_NPC_ROLE
{
    ENUM_NPC_ROLE_0 = 0
};

class CNPCScript
{
public:
    CNPCScript();
    ~CNPCScript();
    bool load(char const* path);
    bool isExistRole(ENUM_NPC_ROLE role) const;
    bool isRewardLevel(unsigned short level) const;
    bool isKeyItem(unsigned long key, unsigned int id) const;
    int getPeriodRewardType() const;
    // ORIG _ZNK10CNPCScript8getIndexEv 0x085819ac W：mov (%eax) —— 返回 +0x00。
    unsigned int getIndex() const { return m_resultCopy; }
    // ORIG _ZNK10CNPCScript14isFavorableNPCEv 0x085819b6 W：movzbl 0x174(%eax)
    // —— 内嵌 NpcScript+0x170 处 stFavorableRelationShip_t::m_enabled。
    bool isFavorableNPC() const
    {
        return m_npcScript.m_favorableRelationShip.m_enabled;
    }


private:
    // +0x00: load 成功后回写的 NpcScript 首字（ORIG load 把 this+4 首字拷到 this）。
    unsigned int m_resultCopy;              // ORIG +0x00
    // +0x04: 内嵌 NpcScript 权威结构（importNpcScript 填充）。
    NpcScript m_npcScript;                  // ORIG +0x04 ..
};

#endif  // GAME_NPCSCRIPT_H
