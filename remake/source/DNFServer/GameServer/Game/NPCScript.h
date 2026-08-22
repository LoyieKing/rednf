#ifndef GAME_NPCSCRIPT_H
#define GAME_NPCSCRIPT_H

// ============================================================================
// CNPCScript / ENUM_NPC_ROLE
//
// 从 GameStubs.cpp 迁移的 G2 桩类。isExistRole 为 Area.cpp 通过 extern "C" asm
// 符号 _ZNK10CNPCScript11isExistRoleE13ENUM_NPC_ROLE 引用，须保留（符号参数类型
// 为枚举标签 ENUM_NPC_ROLE，恰好 13 字符）。
//
// 其余 ORIG getter（nm -C df_game_r 确认，取类型简单者补齐符号，DI/实现为空桩）：
//   _ZN10CNPCScriptC1Ev / D1Ev          CNPCScript() / ~CNPCScript()
//   _ZN10CNPCScript4loadEPKc             load(char const*)
//   _ZNK10CNPCScript13isRewardLevelEt   isRewardLevel(unsigned short) const
//   _ZNK10CNPCScript9isKeyItemEmj        isKeyItem(unsigned long, unsigned) const
//   _ZNK10CNPCScript19getPeriodRewardTypeEv  getPeriodRewardType() const
// ============================================================================

enum ENUM_NPC_ROLE
{
    ENUM_NPC_ROLE_0 = 0
};

class CNPCScript
{
public:
    CNPCScript();
    ~CNPCScript();
    void load(char const* path);
    bool isExistRole(ENUM_NPC_ROLE role) const;
    bool isRewardLevel(unsigned short level) const;
    bool isKeyItem(unsigned long key, unsigned int id) const;
    int getPeriodRewardType() const;
};

#endif  // GAME_NPCSCRIPT_H