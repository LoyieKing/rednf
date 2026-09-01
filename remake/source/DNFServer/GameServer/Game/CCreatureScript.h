// ============================================================================
// df_game_r 还原 —— user_creature::CCreatureScript（CCreatureScriptMgr 持有的
//   runtime 宠物脚本包装类）。布局依据：
//     ctor 0x833c77e / dtor 0x833c848 / Set 0x833c8e8 /
//     GetItemEvolutionCreatureId 0x833cad8 / IsAvailableArtifactSlot 0x833cb38
//   及 0x833f11a..0x833f378 各 W 访问器（GetMaxLevel 0x833f24c 等）。
//   字段偏移与原生 CreatureScript（CreatureScript.h）对应关系见 Set 注释。
// ============================================================================
#ifndef DNF_CCREATURESCRIPT_H
#define DNF_CCREATURESCRIPT_H

#include <set>
#include <vector>

#include "CreatureScript.h"

namespace user_creature
{

class CCreatureScript
{
public:
    CCreatureScript();                 // ORIG 0x833c77e
    ~CCreatureScript();                // ORIG 0x833c848

    // ORIG 0x833c8e8：从原生 CreatureScript* 拷贝字段。
    void Set(CreatureScript* script);

    // ---- 访问器（ORIG W，地址见 nm）----
    int GetItemEvolutionCreatureId(int creatureId);  // 0x833cad8
    unsigned char IsAvailableArtifactSlot(int slot); // 0x833cb38

    void SetItemId(int itemId);        // 0x833f11a：m_itemId(+0x1c)
    int GetItemId();                   // 0x833f128
    bool IsExistEvolutionQuest();      // 0x833f134：m_field0 != 0
    int GetEvolutionCreatureId();      // 0x833f144：m_field4
    bool validEventEvlouteCreature(int creatureId);  // 0x833f150
    bool isEvoluationCreature(int creatureId);       // 0x833f1e0
    int GetEvolutionLevel();           // 0x833f240：m_field14
    int GetMaxLevel();                 // 0x833f24c：m_field18
    int GetSkillRecoveryTime();        // 0x833f258：m_field24
    int GetOverSkillRecoveryTime();    // 0x833f264：m_field28
    int GetLearnOverSkillLevel();      // 0x833f270：m_field2c 字节
    bool validItemExp(int itemIdx);    // 0x833f280
    bool validItem_AType(int itemIdx); // 0x833f2bc
    bool validItem_BType(int itemIdx); // 0x833f2ee
    bool IsItemGrowCreature();         // 0x833f320
    int GetTypeSelectEvolutionLevel(); // 0x833f34a：m_field50
    int GetGrowItemCountforLevelUp();  // 0x833f356：m_field64
    int GET_ATypeCreature();           // 0x833f362：m_field54
    int GET_BTypeCreature();           // 0x833f36e：m_field58

    // ---- 布局（与 Set/访问器一致）----
    int m_field0;                        // +0x00（evolutionQuestId）
    int m_field4;                        // +0x04（evolutionCreatureId）
    std::vector<int> m_vec8;             // +0x08（evolutionMultyId，自 src+0x180）
    int m_field14;                       // +0x14（evolutionLevel）
    int m_field18;                       // +0x18（maxLevel）
    int m_itemId;                        // +0x1c
    char* m_artifactSlotFlags;           // +0x20（new char[3]：red/blue/green）
    int m_field24;                       // +0x24（skillRecoveryTime）
    int m_field28;                       // +0x28（overSkillRecoveryTime）
    unsigned char m_field2c;             // +0x2c（learnOverSkillLevel）
    char m_pad30[0x34 - 0x30];           // +0x30
    std::set<int> m_set34;               // +0x34（itemEvolutionCreatureIds，自 src+0x1c0）
    bool m_field4c;                      // +0x4c（isItemGrow flag，自 src+0x18c；ORIG IsItemGrowCreature movzbl 按 bool 读取）
    char m_pad4d[0x50 - 0x4d];           // +0x4d
    int m_field50;                       // +0x50（typeSelectEvolutionLevel / growLevel）
    int m_field54;                       // +0x54（growATypeCreature）
    int m_field58;                       // +0x58（growBTypeCreature）
    int m_field5c;                       // +0x5c（growATypeItem）
    int m_field60;                       // +0x60（growBTypeItem）
    int m_field64;                       // +0x64（growItemCountforLevelUp）
    std::vector<int> m_vec68;            // +0x68（eventEvlouteCreature，自 src+0x208）
};

}  // namespace user_creature

#endif  // DNF_CCREATURESCRIPT_H
