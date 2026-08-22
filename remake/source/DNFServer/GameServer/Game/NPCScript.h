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
// 布局说明：ORIG CNPCScript 在 +0x04 内嵌一个 NpcScript 对象（含角色 map、
// key item 表、奖励等级表等）。该 NpcScript 结构本工程尚未还原，故仅在
// 本类内用同型 std 容器按 ORIG 字段顺序建模访问器所需的数据区；isExistRole
// 所需的角色 map ORIG 位于 +0x50。完整 NpcScript 重建前，成员间的确切字节
// 偏移为「推断」，仅保证类型/顺序与 ORIG 一致。
// ============================================================================

#include <map>
#include <string>
#include <utility>
#include <vector>

enum ENUM_NPC_ROLE
{
    ENUM_NPC_ROLE_0 = 0
};

// ORIG: stFavorableRelationShip_t::stLevelRewardInfo（奖励等级区间条目）。
// 首字段为 ushort 等级（isRewardLevel/getLevelRewardInfo 均以该字段比较）。
struct CNPCScript_LevelRewardInfo
{
    unsigned short m_level;   // +0x00 等级
    unsigned int   m_flag0;   // +0x04 扩展标志
    unsigned int   m_flag1;   // +0x08 扩展标志
    std::string    m_name;    // +0x0c
    std::string    m_desc;    // +0x10
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

private:
    // +0x00: load 成功后回写的 NpcScript 首字（ORIG load 把 this+4 首字拷到 this）。
    unsigned int m_resultCopy;              // ORIG +0x00
    // +0x04: 内嵌 NpcScript 对象（ORIG 曝光 +0x04）。仅 load 以
    //        importNpcScript(NpcScript*, path) 填充并回读首字；结构未还原，
    //        用不透明字节区占位表示，读取经 reinterpret_cast。
    char m_npcScriptData[0x4c];             // ORIG +0x04 .. +0x50
    // +0x50: isExistRole 查询的角色标志。ORIG 0x86d2736:
    //        std::map<ENUM_NPC_ROLE,int>::find(role) != end()。
    std::map<ENUM_NPC_ROLE, int> m_roleMap; // ORIG +0x50
    // +0x190: isKeyItem 查询的 key item 表（itemKey, itemId）。
    std::vector<std::pair<unsigned long, unsigned int> > m_keyItemList; // ORIG +0x190
    // +0x658: isRewardLevel 查询的奖励等级区间表。
    std::vector<CNPCScript_LevelRewardInfo> m_levelRewardList;          // ORIG +0x658
    // +0x674: getPeriodRewardType 比较的周期奖励类型（"item"/"quest"）。
    std::string m_periodRewardType;         // ORIG +0x674
};

#endif  // GAME_NPCSCRIPT_H