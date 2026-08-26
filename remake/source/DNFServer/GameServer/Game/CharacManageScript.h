// ============================================================================
// CharacManageScript.h — charac_manage.etc 脚本宿主（ORIG sizeof 0x88）
//
// 依据 docs/class_func_reports/CharacManageScript/{getInsance,importScript}.md
// 与 ORIG 反汇编：
//   CharacManageScript::CharacManageScript T 0x088e06cc（逐容器构造 + clear）
//   getInsance                             T 0x088e0674（new 0x88 单例 instance_）
//   importScript                           T 0x088e0968
// 成员偏移全部来自 ctor/importScript 汇编实证。
// 此前 CDataManager.cpp 内的最小镜像由本头取代（专属批次 Ctor_Group2）。
// ============================================================================
#ifndef GAME_CHARAC_MANAGE_SCRIPT_H_
#define GAME_CHARAC_MANAGE_SCRIPT_H_

#include <map>
#include <string>
#include <vector>

// ---- ManagePointBonus（[point bonus] 行；3×int，ORIG 栈对象 0xc）----
struct ManagePointBonus
{
    int m_field00;  // +0x00
    int m_field04;  // +0x04
    int m_field08;  // +0x08
};  // 0xc

// ---- ContentsOptionRevision（默认分支行；2×int，ORIG 栈对象 8 字节）----
struct ContentsOptionRevision
{
    int m_type;    // +0x00（ContentsType::getType）
    int m_field04; // +0x04
};  // 0x8

class CharacManageScript
{
public:
    static CharacManageScript* getInsance();   // ORIG _ZN18CharacManageScript10getInsanceEv
    static CharacManageScript* instance_;      // ORIG _ZN18CharacManageScript9instance_E B 0x094f6b98

    CharacManageScript();                      // ORIG T 0x088e06cc
    ~CharacManageScript();                     // ORIG T 0x088e07f6（逆序容器析构）
    void clear();                              // ORIG _ZN18CharacManageScript5clearEv（ctor 调用）

    bool importScript(const char* path);       // ORIG T 0x088e0968

    float GetExpBonus(int level);              // ORIG T 0x088e0f58（m_expBonus[level]/0.1f）
    float GetGoldBonus(int level);             // ORIG T 0x088e0fce（m_goldBonus[level]/0.1f）
                                               // 实现暂居 CPartyDepsD2.cpp（PartyDepsD2 批次，
                                               // CParty::CalcurateManageBonus 依赖闭环）
    int GetLevel(int point);     // ORIG T 0x088e0ed4（定义 CUser.cpp，LinkMisc5）
    int GetPoint(int level);     // ORIG T 0x088e0e0a（同上）
    int GetLevelUpRequirePoint(unsigned int maxLevel);
                                 // ORIG T 0x088e1114（同上）

    // ---- 布局（ctor 0x088e06cc / importScript 实证）----
    std::vector<ManagePointBonus> m_pointBonus;      // +0x00（[point bonus]）
    std::vector<int> m_manageLevelPoint;             // +0x0c（[manage level point]）
    std::map<int, int> m_expBonus;                   // +0x18（[exp bonus]，insert 查重）
    std::map<int, int> m_goldBonus;                  // +0x30（[gold bonus]，insert 查重）
    int m_manageLevelMax;                            // +0x48（[manage level max]）
    std::map<int, std::string> m_basicExplain;       // +0x4c（[manage basic explain]）
    std::map<int, int> m_manageOption;               // +0x64（[manage option]）
    std::vector<ContentsOptionRevision> m_optionRevision;  // +0x7c（默认分支）
};  // sizeof == 0x88

#endif  // GAME_CHARAC_MANAGE_SCRIPT_H_
