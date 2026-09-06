#ifndef GAME_EVENTUPGRADEJARINFO_H_
#define GAME_EVENTUPGRADEJARINFO_H_

// ============================================================================
// df_game_r 还原 —— Event_Upgrade_Jar（Event Upgrade Jar）与 EventUpgradeJarInfo。
// 布局依据 ORIG 反汇编（df_game_r，elf32-i386）。
//
// 继承/复合关系：
//   Event_Upgrade_Jar（_ZN17Event_Upgrade_Jar）继承自 EventUpgradeJarInfo
//   （_ZN19EventUpgradeJarInfo）：
//     - Event_Upgrade_Jar::Event_Upgrade_Jar（0x080f49b8）仅转发：
//       EventUpgradeJarInfo::EventUpgradeJarInfo((EventUpgradeJarInfo*)this)
//     - Event_Upgrade_Jar::~Event_Upgrade_Jar（0x080f49cc）仅转发：
//       EventUpgradeJarInfo::~EventUpgradeJarInfo((EventUpgradeJarInfo*)this)
//   因此 Event_Upgrade_Jar 无自己的数据成员，全部字段在 EventUpgradeJarInfo 基类。
//
// EventUpgradeJarInfo 布局（ctor 0x080f50a4 = 0x080f504f+0x55，dtor 0x080f51d2）：
//   +0x00  ushort                     m_npcCount        （[group info] 首行 ScanInt）
//   +0x04  map<unsigned long,short>   m_boosterIndexMap （[group info] 每行索引 map）
//   +0x1c  map<pair<int,int>,ulong>   m_recipeMap       （[group info] 强化配方 map）
//   +0x34  vector<unsigned long>      m_materialIndices （[fortune index] 材料索引）
//   +0x40  vector<unsigned int>[4]    m_failRateByFortune（[fortune fail rate] 4 组分段）
//                                  数组元素 stride = 0x0c（vector<uint> 0x0c）
//   +0x70  float                      m_discountRate     （[event upgrade table] 首值）
//   +0x74  vector<ST_Event_Upgrade_Jar_EventNPCs_Script> m_npcScripts（[event upgrade table]）
//   +0x80  int                        m_upgradeMax       （[upgrade max]）
//   +0x84  int                        m_materialKind     （0=none/1=fortune/2=newNPC，运行时置位）
//   +0x88  uchar                      m_materialIdx      （运行时置位）
//   总大小 0x8c。
//
// 注：m_materialKind/m_materialIdx（+0x84/+0x88）为工作状态，ctor 不初始化，由
// Event_Upgrade_Jar::CheckMaterialIndex / SettingRecipeNUpConunt 等在运行时写入。
// ============================================================================

#include <vector>
#include <map>
#include <utility>

#include "Inven_Item.h"
#include "CItemAmplifier.h"   // upgrade_info_t 权威定义（0x6c）

// ---- ST_Event_Upgrade_Jar_EventNPCs_Script（[event upgrade table] NPC 行）----
// ctor 0x08968ec2 清零两个 uint；字段由 importEventUpgradeJarScript 解析写入：
//   +0x00 uint m_failRate（_getFailProb_NewNPC 读 *elem）
//   +0x04 uint m_meterialCount（_getMeterialCount_NewNPC 读 elem+4）
struct ST_Event_Upgrade_Jar_EventNPCs_Script
{
    ST_Event_Upgrade_Jar_EventNPCs_Script() : m_failRate(0), m_meterialCount(0) {}

    unsigned int m_failRate;      // +0x00
    unsigned int m_meterialCount; // +0x04
};

class EventUpgradeJarInfo
{
public:
    EventUpgradeJarInfo();          // ORIG 0x080f50a4（W）
    ~EventUpgradeJarInfo();         // ORIG 0x080f51d2（W）

    // ---- 数据（字段偏移见头注释）----
    unsigned short m_npcCount;      // +0x00
    std::map<unsigned long, short > m_boosterIndexMap;        // +0x04
    std::map<std::pair<int, int>, unsigned long> m_recipeMap; // +0x1c
    std::vector<unsigned long > m_materialIndices;            // +0x34
    std::vector<unsigned int > m_failRateByFortune[4];        // +0x40 步长 0x0c
    float m_discountRate;                                     // +0x70
    std::vector<ST_Event_Upgrade_Jar_EventNPCs_Script > m_npcScripts; // +0x74
    int m_upgradeMax;                                         // +0x80
    int m_materialKind;                                       // +0x84 运行时
    unsigned char m_materialIdx;                              // +0x88 运行时
};

// ---- Event_Upgrade_Jar（继承 EventUpgradeJarInfo；无自有数据成员）----
class Event_Upgrade_Jar : public EventUpgradeJarInfo
{
public:
    Event_Upgrade_Jar();                                        // ORIG 0x080f49b8 C1
    ~Event_Upgrade_Jar();                                       // ORIG 0x080f49cc D1

    int LoadScript(const char* scriptDir, const char* path);     // 0x080f49e0（P9：bool→int，无 xor 物化）
    bool check_requiredRecipe(const Inven_Item& item) const;     // 0x080f4bb0
    unsigned int _getFailProb_Fortune(int materialIdx, unsigned int upgrade) const; // 0x080f4be0
    unsigned int _getFailProb_NewNPC(unsigned int idx) const;    // 0x080f4c8e
    unsigned int _getMeterialCount_NewNPC(unsigned int idx);     // 0x080f4ccc
    float _getDiscountRate_NewNPC() const;                       // 0x080f4d0c
    void CheckMaterialIndex(unsigned long index);                // 0x080f4d20
    void SettingRecipeNUpConunt(int materialIdx, upgrade_info_t& info, unsigned char upgrade); // 0x080f4dac
    void GetProbability(int materialIdx, unsigned int& outProb) const; // 0x080f4ee2
    bool CheckMaxLuckyLevel(Inven_Item& item, unsigned long upgrade);       // 0x080f4f62（ORIG _ZN 非 const）
    unsigned long GetIndexLuckyStrengtheningDeed();              // 0x080f4fd0
    unsigned long getBoosterIndex_byUpgradeCardIndex(unsigned long& cardIdx, unsigned long& boosterIdx); // 0x080f4a0a
    bool isFortune() const;                                     // 0x0854b57c（W）
    bool isNewNPC() const;                                      // 0x0854b590（W）
};

// ---- 自由函数（<global>）----
bool importEventUpgradeJarScript(EventUpgradeJarInfo* info, const char* path);   // 0x089687d1
bool updateUpgradeTableFromEventUpgradeJarScript(upgrade_table_t* table, EventUpgradeJarInfo* info); // 0x08968d59

#endif  // GAME_EVENTUPGRADEJARINFO_H_
