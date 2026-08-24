// ============================================================================
// CItemUpgrade_Separate.h — WongWork 分离强化系统（ORIG sizeof 0x194）
//
// 依据 docs/class_func_reports/{WongWork__CItemUpgrade_Separate,WongWork__IItemUpgrade,
// GenuineDamageUpgradeTable}/ 与 ORIG 反汇编/nm：
//   WongWork::IItemUpgrade      ctor T 0x080fc834 / dtor T 0x080fc842（仅置 vptr；
//                               nm 无 D0 → 析构非虚）/ item_upgrade_system
//                               T 0x080fc850 / vtable 0x08b3fa30 双纯虚槽
//                               （槽序 ProcUpgrade→LoadUpgradeScript 由派生类
//                               vtable 0x08b56948 实证）
//   GenuineDamageUpgradeTable   ctor W 0x0811ee5e / dtor W 0x0811eeda（三 map 构造/
//                               析构；头区 0x140 不触碰）sizeof 0x188
//   GenuineDamageUpgradeInfo    ctor W 0x0891baf0（清零前 4 dword）；字段由
//                               getGenuineUpgradeMaterialCnt(+0x10 float)/
//                               _GetUpgradeInfo(+0xc float)/getGenuineUpgradeDamage
//                               (+0/+4 int) 实证，sizeof 0x14
//   GenuineDamageUpgradeMaterialInfo  ORIG 无任何符号（纯 POD）；
//                               getMaterialCnt/_CheckCondition 读 int@+8，
//                               _CheckCondition 读 int@+4 → sizeof ≥ 0xc
//   OutLineEFFECTInfoTemp       ctor W 0x0891bb1e（+8 写两次）；getEffectInfo
//                               拷贝 4 dword → sizeof 0x10
//   STUpgradeInfo               ctor W 0x0811edfe（清零双 int）sizeof 8
//   CItemUpgrade_Separate()     T 0x0811dfc0 / D1 T 0x0811e0be /
//                               LoadUpgradeScript T 0x0811eca8（import 尾透传）/
//                               ProcUpgrade T 0x0811e66e / getUpgradeInfoTable
//                               W 0x0811ee16（return this+4）
// ============================================================================
#ifndef GAME_CITEM_UPGRADE_SEPARATE_H_
#define GAME_CITEM_UPGRADE_SEPARATE_H_

#include <map>

// ENUM_RARITY / ENUM_EQUIPMENTTYPE（ORIG mangled 名称所需；两处均带防重 guard）
#include "CItemAmplifier.h"
#include "STEquipmentScript.h"
class CUser;
class Inven_Item;
struct MSG_ITEM_UPGRADE;
class PacketGuard;

class CMTRand;  // PvPTypes.h（0x9c8 MT19937）


// ---- GenuineDamageUpgradeTable 元素 POD（偏移经 importGenuineUpgradeTable
// 报告写入点与 getMaterialCnt/getGenuineUpgradeDamage 汇编读取点双向实证）----
struct GenuineDamageUpgradeMaterialInfo
{
    int m_pad00;             // +0x00（import 写入；本域无读者）
    int m_materialItemIdx;   // +0x04（getMaterialCnt 经 pair+8 即 value+4 读作
                             //      材料数基数，fildl × factor）
};  // sizeof == 0x8

static_assert(sizeof(GenuineDamageUpgradeMaterialInfo) == 0x8,
              "GenuineDamageUpgradeMaterialInfo size drift");

struct GenuineDamageUpgradeInfo
{
    GenuineDamageUpgradeInfo();              // ORIG W 0x0891baf0：清零前 4 dword

    int m_a;                                 // +0x00（useAlt=false 分支乘数，fildl）
    int m_b;                                 // +0x04（useAlt=true 分支乘数，fildl）
    float m_levelRate;                       // +0x08（import：level/10000f；ctor 清零）
    float m_goldRate;                        // +0x0c（_GetUpgradeInfo ×9840f → 金币）
    float m_materialRate;                    // +0x10（getMaterialCnt 放大系数；ctor 不初始化）
};  // sizeof == 0x14

static_assert(sizeof(GenuineDamageUpgradeInfo) == 0x14,
              "GenuineDamageUpgradeInfo size drift");

struct OutLineEFFECTInfoTemp
{
    OutLineEFFECTInfoTemp();                 // ORIG W 0x0891bb1e（+0/+4/+8 置 0，+8 两次）

    int m_a;                                 // +0x00
    int m_b;                                 // +0x04
    int m_c;                                 // +0x08（ctor 写两次的槽）
    int m_d;                                 // +0x0c（ctor 不初始化；getEffectInfo 随值返回）
};  // sizeof == 0x10

static_assert(sizeof(OutLineEFFECTInfoTemp) == 0x10,
              "OutLineEFFECTInfoTemp size drift");

namespace WongWork
{

// ---- 分离强化条目（ORIG C1 W 0x0811edfe 清零双 int）----
struct STUpgradeInfo
{
    STUpgradeInfo();

    int m_materialCnt;                       // +0x00（消耗材料组数）
    int m_gold;                              // +0x04（trunc(9840f * info.goldRate)）
};  // sizeof == 8

}  // namespace WongWork

// ---- GenuineDamageUpgradeTable（全局类；ORIG ctor/dtor 为 W 内联 emitted）----
class GenuineDamageUpgradeTable
{
public:
    GenuineDamageUpgradeTable();             // ORIG W 0x0811ee5e
    ~GenuineDamageUpgradeTable();            // ORIG W 0x0811eeda

    // ORIG T 0x08912d10（sret 返回；静态局部 temp 经 __cxa_guard 初始化）
    OutLineEFFECTInfoTemp getEffectInfo(int key);
    // ORIG T 0x08912a54：m_infos[key]（operator[] 可插入）× 双浮点表 × 计数比例。
    int getGenuineUpgradeDamage(ENUM_RARITY rarity, int a, int b, int key,
                                int count, bool useAlt);
    // ORIG T(_ZNK…Ehi18…11…) 0x08912b88：type!=10 / growth>100 / find 失败 → -1。
    int getGenuineUpgradeMaterialCnt(unsigned char growth, int sepLv,
                                     ENUM_EQUIPMENTTYPE type,
                                     ENUM_RARITY rarity) const;

    // ---- 头区布局（getter/_CheckCondition/_GetUpgradeInfo 访问实证）----
    int m_divisor;                           // +0x000（getGenuineUpgradeDamage 分母 *(int*)this）
    int m_maxLevel;                          // +0x004（分离上限；_CheckCondition 0x5f 判定）
    float m_itemWeightsByRarity[6];          // +0x008..+0x01f（[item weights by rarity]；
                                             //   getMaterialCnt 因子源 this+(rarity)*4+8）
    float m_statWeightsByRarity[6];          // +0x020..+0x037（[stat weights by rarity]；
                                             //   getGenuineUpgradeDamage 读 this+(rarity+8)*4）
    float m_weaponWeights[11][6];            // +0x038..+0x13f（[weapon weights]；
                                             //   getGenuineUpgradeDamage 读 this+((6a+b)+12)*4+8）
    std::map<int, GenuineDamageUpgradeMaterialInfo> m_materials;  // +0x140
    std::map<int, GenuineDamageUpgradeInfo> m_infos;              // +0x158
    std::map<int, OutLineEFFECTInfoTemp> m_effects;               // +0x170
};  // sizeof == 0x188

static_assert(sizeof(GenuineDamageUpgradeTable) == 0x188,
              "GenuineDamageUpgradeTable size drift");

bool importGenuineUpgradeTable(const char* path,
                               GenuineDamageUpgradeTable* table);
    // ORIG T 0x08912336（eax 流出 bool；定义 ScriptDepsB.cpp）

namespace WongWork
{

// ---- IItemUpgrade（抽象基类；nm 仅 C1/C2/D1/D2 → 析构非虚）----
class IItemUpgrade
{
public:
    virtual int ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                            PacketGuard* packet) = 0;          // 槽 +0x00
    virtual int LoadUpgradeScript(const char* path) = 0;       // 槽 +0x04

    // ORIG T 0x080fc850：put_header(pg,1,cmd) → 虚调 ProcUpgrade → 成功 finalize+
    // Send，失败 SendCmdErrorPacket2Byte(cmd, ret&0xffff)。
    void item_upgrade_system(CUser* user, MSG_ITEM_UPGRADE* msg, int cmd);

    ~IItemUpgrade();   // 非虚（ORIG 无 D0 符号；CDataManager 持派生类指针删除）

protected:
    IItemUpgrade();    // ORIG T 0x080fc834：仅置 vptr
};

// ---- CItemUpgrade_Separate（sizeof == 0x194）----
class CItemUpgrade_Separate : public IItemUpgrade
{
public:
    CItemUpgrade_Separate();                   // ORIG T 0x0811dfc0
    ~CItemUpgrade_Separate();                  // ORIG T 0x0811e0be

    virtual int ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                            PacketGuard* packet);              // ORIG T 0x0811e66e
    virtual int LoadUpgradeScript(const char* path);           // ORIG T 0x0811eca8

    // ORIG W 0x0811ee16：return &m_table（this+4）。
    const GenuineDamageUpgradeTable* getUpgradeInfoTable() const { return &m_table; }

private:
    // ORIG T 0x0811e138（const）：材料/成长度/耐久/上限校验，返回错误码或 0。
    int _CheckCondition(const CUser* user, const Inven_Item& item,
                        const CItem* citem, const Inven_Item& matItem,
                        const GenuineDamageUpgradeTable& table, int level) const;
    // ORIG T 0x0811e2ce：恒 return 1（报告 _GetPenaltyType.md 全文仅此）。
    int _GetPenaltyType(unsigned char a, ENUM_RARITY rarity,
                        const GenuineDamageUpgradeTable& table,
                        const GenuineDamageUpgradeInfo* info);
    // ORIG T 0x0811e2e2：查 sepLv+1 的 info 行并填 STUpgradeInfo（材料数+金币）。
    bool _GetUpgradeInfo(const Inven_Item* item, const CItem* citem,
                         const GenuineDamageUpgradeTable& table,
                         STUpgradeInfo* out, int level);
    // ORIG T 0x0811e468：GM 直改通道 + gold*10 对 randInt(m_randRange) 成败判定。
    bool _DoProcUpgrade(CUser* user, Inven_Item* item, const STUpgradeInfo* info);
    // ORIG T 0x0811e53a：header 0x56 全服广播（sep>5 时调用）。
    void _NoticeUpgrade(const CUser* user, const Inven_Item& item, bool success);

    // ---- 布局（ctor/dtor 实证）----
    /* IItemUpgrade: vptr @+0x00 */
    GenuineDamageUpgradeTable m_table;  // +0x04（0x188）
    CMTRand* m_rand;                    // +0x18c
    int m_randRange;                    // +0x190（ctor=100000 热身上界）
};  // sizeof == 0x194

}  // namespace WongWork

#endif  // GAME_CITEM_UPGRADE_SEPARATE_H_
