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
    // ORIG W ctor 0x088e12f6（C2 同址）：三 int 清零。
    ManagePointBonus() { m_field00 = 0; m_field04 = 0; m_field08 = 0; }
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
    void GetExpBonus(int level, int* out);     // ORIG T 0x088e1044（命中写 *out，未命中不写）
    float GetGoldBonus(int level);             // ORIG T 0x088e0fce（m_goldBonus[level]/0.1f）
    void GetGoldBonus(int level, int* out);    // ORIG T 0x088e10ac（命中写 *out，未命中不写）
    int GetLevel(int point);                   // ORIG T 0x088e0ed4
    int GetPoint(int level);                   // ORIG T 0x088e0e0a
    int GetLevelUpRequirePoint(unsigned int maxLevel);  // ORIG T 0x088e1114
    int GetMinimumPointCharacterLevel();       // ORIG T 0x088e1266（m_pointBonus 空 → 0x28）
    std::string GetName(unsigned int key);     // ORIG T 0x088e1166（m_basicExplain[key]）
    int GetOptionValue(int type);              // ORIG T 0x088e11fc（m_manageOption[type]）
                                               // 实现见 CharacManageScript.cpp

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
