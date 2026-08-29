// ============================================================================
// WongWorkDrop_A.cpp — G1 Drop 家族（WongWorkA 批次）逐报告忠实转写
//
// 实现 16 个 ORIG 符号（唯一 owner = 本 TU）：
//   CWorldDrop::generateRandomItem              T 0x085383b8
//   CDungeonDrop::generateSpecificItem          T 0x08539c3e
//   CObjectDrop::generateRandomItem             T 0x0853818a
//   CObjectDrop::generateSpecificItem           T 0x08538234
//   CMonsterDrop::setDropRatio                  T 0x08535cf8
//   CMonsterDrop::_getBossGoldDrop              T 0x08535dc8
//   CMonsterDrop::_getGenItemProbIdx            T 0x085372ee
//   CMonsterDrop::_generateRandomItem           T 0x08535ed2
//   CMonsterDrop::_generateMonseterSpecificItem T 0x08536fce（ORIG 拼写如此）
//   CMonsterDrop::generateRandomItem            T 0x08536da8
//   CMonsterDrop::generateSpecificItem          T 0x08537070
//   stFirstMobHuntingWeight_t::stFirstMobHuntingWeight_t W 0x0853a6ae
//
// 引用但非本批符号（本体待所属批次产出；__asm__ 标签直链 ORIG mangled 名，
// 参照 GmCmds_D.cpp 同型桥。this 作首参入栈 = GNU i386 成员 ABI）：
//   CMonsterDrop_Hell::{setDropRatio,generateSpecificItem,hellPartySpecificItem}
//   CSpecialMonsterDrop::{setDropRatio,_generateRandomItem}
//   CAutoMarketConditionsControl::{GetItemDropRate,ChangeItemRarity}
//   ::CLuckPoint::{UseLuckPoint(成员4int),sendItemDropRate}
//   GenerateRandomUpgrade::{generateEquipGrade,generateUpgrade}
//   CMonster::{drop_item_index(ji),generateDropAvatarItem,generateCatchMonsterItem}
//   CAICharacter::generateDropAvatarItem
//   ::roundFloat 0x0853abe5（定义于 CClearReward.cpp）
//
// stGenerateRefData_t +0x34..+0x47 为调用方上下文 blob（头注释实证）；本文件
// 按字节偏移读取并注明语义【推断】，不改任何共享布局。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include <algorithm>

#include "CItemGenerator.h"
#include "Inven_Item.h"
#include "GameRand.h"
#include "CDataManager.h"
#include "CMonster.h"
#include "STMonsterScript.h"
#include "MonsterIndependentDrop.h"
#include "CDungeon.h"

namespace WongWork
{


}  // namespace WongWork

// ORIG W 0x0853abe5 ::roundFloat —— 定义于 CClearReward.cpp（drops 域公共依赖，
// 全局命名空间符号 _Z10roundFloatf；声明须置于全局域保证调用 mangled 匹配）
extern float roundFloat(float v);

namespace WongWork
{

// ---------------------------------------------------------------------------
// 文件局部 asm 桥（只引用不定义）
// ---------------------------------------------------------------------------
namespace {

void WM_Hell_setDropRatio(CMonsterDrop_Hell* self, float ratio)
    __asm__("_ZN8WongWork17CMonsterDrop_Hell12setDropRatioEf");
void WM_Special_setDropRatio(CSpecialMonsterDrop* self, float ratio)
    __asm__("_ZN8WongWork19CSpecialMonsterDrop12setDropRatioEf");
void WM_Hell_generateSpecificItem(CMonsterDrop_Hell* self,
                                  stGenerateRefData_t const& ref,
                                  stGenerateResult_t& result)
    __asm__("_ZN8WongWork17CMonsterDrop_Hell20generateSpecificItemERKNS_19stGenerateRefData_tE"
            "RNS_18stGenerateResult_tE");
void WM_Hell_hellPartySpecificItem(CMonsterDrop_Hell* self,
                                   stGenerateRefData_t const& ref,
                                   stGenerateResult_t& result)
    __asm__("_ZN8WongWork17CMonsterDrop_Hell21hellPartySpecificItemERKNS_19stGenerateRefData_tE"
            "RNS_18stGenerateResult_tE");
void WM_Special_generateRandomItem(
    CSpecialMonsterDrop* self, unsigned char type, char role,
    unsigned char dungeonLv, unsigned char difficulty, unsigned char partyPos,
    int context3c, unsigned char hellFlag, int dropRate,
    stGenerateResult_t& result, bool& done)
    __asm__("_ZN8WongWork19CSpecialMonsterDrop19_generateRandomItemEhchhhihiRNS_18stGenerateResult_tERb");

float AM_GetItemDropRate(void* amc, int probIdx)
    __asm__("_ZN28CAutoMarketConditionsControl15GetItemDropRateEi");
bool AM_ChangeItemRarity(void* amc, int* tableInOut, int* outTable,
                         int rarityIdx, double factor)
    __asm__("_ZN28CAutoMarketConditionsControl16ChangeItemRarityEPiS0_id");

bool Luck_UseLuckPointM(::CLuckPoint* lp, int itemType, int rarity, int grade,
                        int seq)
    __asm__("_ZN10CLuckPoint12UseLuckPointEiiii");
void Luck_sendItemDropRate(::CLuckPoint* lp, char const* text)
    __asm__("_ZN10CLuckPoint16sendItemDropRateEPKc");

int Upg_generateEquipGrade(GenerateRandomUpgrade* self, int levelDelta)
    __asm__("_ZN8WongWork21GenerateRandomUpgrade18generateEquipGradeEi");
int Upg_generateUpgrade(GenerateRandomUpgrade* self, ENUM_RARITY rarity,
                        int levelDelta)
    __asm__("_ZN8WongWork21GenerateRandomUpgrade15generateUpgradeE11ENUM_RARITYi");

int Mon_drop_item_index2(CMonster const* self, unsigned int idxType, int roll)
    __asm__("_ZNK8CMonster15drop_item_indexEji");
void Mon_generateDropAvatarItem(CMonster const* self, unsigned char a,
                                unsigned char b,
                                std::vector<Inven_Item>& out)
    __asm__("_ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE");
void Mon_generateCatchMonsterItem(CMonster const* self,
                                  std::vector<Inven_Item>& out)
    __asm__("_ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE");
void AI_generateDropAvatarItem(CAICharacter const* self, unsigned char a,
                               unsigned char b,
                               std::vector<Inven_Item>& out)
    __asm__("_ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE");

// 命中即建道具入结果（ORIG 各处同型四连）
inline void pushCreatedItem(int itemIdx, stGenerateResult_t& result)
{
    Inven_Item inv;                                    // C2（ORIG 内联）
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    result.m_dropItems.push_back(inv);
}

}  // namespace

// ref blob 字节读取 helper（+0x34..+0x47 调用上下文区【推断语义】）
inline unsigned char u8_at(stGenerateRefData_t const& ref, int off)
{
    return *reinterpret_cast<unsigned char const*>(
        reinterpret_cast<char const*>(&ref) + off);
}
inline signed char i8_at(stGenerateRefData_t const& ref, int off)
{
    return *reinterpret_cast<signed char const*>(
        reinterpret_cast<char const*>(&ref) + off);
}

// ===========================================================================
// CWorldDrop::generateRandomItem — ORIG T 0x085383b8
// ===========================================================================
void CWorldDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                    stGenerateResult_t& result)
{
    // local_24 = &ref.m_field34 视图；+0x34=type +0x35=world/level【推断】
    WorldDropInfo* slot;
    int roll = m_random.generateNumber(0, 100000);     // CGenerateRandomNumber(this+8)

    if (u8_at(ref, 0x35) > 199)                        // ORIG movzbl cmp $0xc7 ja
        return;

    if (u8_at(ref, 0x34) == 1)                         // warArea 分支
        slot = &m_warAreaDrop[u8_at(ref, 0x35)];       // this+level*0x14+0xfcc
    else
        slot = &m_worldDrop[u8_at(ref, 0x35)];         // this+level*0x14+0x18
    // threshold = first × dropRate / 100（ORIG imul 0x51eb851f 魔 数除法，带符号）
    int prod = slot->m_first * ref.m_dropRate;
    int threshold;
    if (prod >= 0)
        threshold = prod / 100;
    else
        threshold = -((-prod + 99) / 100);
    if (roll > threshold)                              // setg 比较后 jg 退出
        return;

    int pick = m_random.generateNumber(1, slot->m_total);   // roll ≤ total 区间
    int acc = 0;                                       // local_14
    for (size_t i = 0; i < slot->m_chunks.size(); ++i)
    {
        acc += slot->m_chunks[i].m_rate;               // 元素+0x04 权重
        if (pick < acc)
        {
            pushCreatedItem(slot->m_chunks[i].m_itemIdx, result);  // 元素+0x00 idx
            return;
        }
    }
}

// ===========================================================================
// CObjectDrop::generateRandomItem — ORIG T 0x0853818a
// ===========================================================================
void CObjectDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                     stGenerateResult_t& result)
{
    bool done = false;                                  // local_21
    for (int n = 0; n < i8_at(ref, 0x37); ++n)          // (char)(blob+0x37) 次数
    {
        unsigned char type = u8_at(ref, 0x34);
        _generateRandomItem(type, 0,                    /* role 固定 '\0' */
                            type,                       /* dungeonLv 复位为同值 */
                            u8_at(ref, 0x35), u8_at(ref, 0x36),
                            0,                          /* context3c = NULL */
                            ref.m_dropRate, result, done);
    }
}

// ===========================================================================
// CObjectDrop::generateSpecificItem — ORIG T 0x08538234
// ===========================================================================
void CObjectDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                       stGenerateResult_t& result)
{
    // 调用方在 ref+0x34 dword 写入 list<pair<int,int>> 配置指针
    std::list<std::pair<int, int> > const* props =
        *reinterpret_cast<std::list<std::pair<int, int> > const* const*>(
            reinterpret_cast<char const*>(&ref) + 0x34);

    for (int n = 0; n < i8_at(ref, 0x39); ++n)
    {
        int pick = m_random.generateNumber(2, 10000);
        int acc = 0;
        for (std::list<std::pair<int, int> >::const_iterator it =
                 props->begin();
             it != props->end(); ++it)
        {
            acc += it->second;                          // +4 权重
            if (pick < acc)
            {
                pushCreatedItem(it->first, result);
                break;
            }
        }
    }
}

// ===========================================================================
// CDungeonDrop::generateSpecificItem — ORIG T 0x08539c3e
// ===========================================================================
void CDungeonDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    // 调用方在 ref+0x34 dword 写入 vector<pair<int,int>> 配置指针
    std::vector<std::pair<int, int> > const* props =
        *reinterpret_cast<std::vector<std::pair<int, int> > const* const*>(
            reinterpret_cast<char const*>(&ref) + 0x34);

    int pick = m_random.generateNumber(1, 10000);
    int acc = 0;
    for (std::vector<std::pair<int, int> >::const_iterator it = props->begin();
         it != props->end(); ++it)
    {
        acc += it->second;                             // +4 权重
        if (pick < acc)
        {
            pushCreatedItem(it->first, result);
            return;
        }
    }
}

// ===========================================================================
// CMonsterDrop
// ===========================================================================

// ---- _getGenItemProbIdx — ORIG T 0x085372ee ----
int CMonsterDrop::_getGenItemProbIdx(int value)
{
    for (size_t i = 0; i < m_probsSub.size(); ++i)     // +0x24 工作副本表
    {
        if (!(value < m_probsSub[i].v[0] || m_probsSub[i].v[1] < value))
            return static_cast<int>(i);
    }
    return -1;
}

// ---- _getBossGoldDrop — ORIG T 0x08535dc8 ----
bool CMonsterDrop::_getBossGoldDrop(unsigned char type, unsigned short& outGold,
                                    unsigned short& outCount)
{
    if (type >= 4)
        return false;

    // cond[type]：首 u16(0x259388+idx*4)=数量基数槽；次 u16(+2)=概率槽【推断】
    unsigned short probSlot =
        *reinterpret_cast<unsigned short const*>(
            reinterpret_cast<unsigned char const*>(&m_conditionRate[type]) + 2);
    unsigned short baseSlot = m_conditionRate[type].rate;

    int tier = 0;
    int roll = m_random.generateNumber(0, baseSlot + m_goldQuantity[2 * type]);
    if (static_cast<int>(probSlot) > roll)             // setg：概率 > roll → 高档
        tier = 1;

    int goldLo = m_goldQuantity[2 * tier];
    int goldHi = m_goldQuantity[2 * tier + 1];
    outGold =
        static_cast<unsigned short>(m_random.generateNumber(0, goldHi - goldLo) +
                                    goldLo);
    outCount = m_goldVolume[tier];                     // (tier+0x12c9d0)*2 → 步长2
    return true;
}

// ---- setDropRatio — ORIG T 0x08535cf8 ----
void CMonsterDrop::setDropRatio(float ratio)
{
    m_probsSub = m_probs;                              // +0x24 ← +0x18（operator=）
    for (size_t i = 0; i < m_probsSub.size(); ++i)
    {
        for (int r = 0; r < 5; ++r)
        {
            // v[2+r]（元素 +8 + r*4）：五档稀有度权重 ×ratio 后回写
            m_probsSub[i].v[2 + r] =
                static_cast<int>(roundFloat(static_cast<float>(m_probsSub[i].v[2 + r]) *
                                            ratio));
        }
    }
    WM_Special_setDropRatio(&m_special, ratio);        // Special (+0x4706c) 先
    WM_Hell_setDropRatio(&m_hell, ratio);              // Hell (+0x238ac) 后
}

// ---- _generateRandomItem — ORIG T 0x08535ed2 ----
// 参数（自 wrapper）【推断命名】：
//   type       = ref+0x34 怪物等级档
//   role       = ref+0x35（signed char；==3 走首领金币路径）
//   dungeonLv  = ref+0x36 地下城等级档
//   difficulty = ref+0x37 难度档
//   partyPos   = ref+0x38 组队档位（-1 后作加成列下标）
//   context3c  = *(int*)&ref[0x3c]（人数上下文，clip [0,100]）
//   dropRate   = ref.m_dropRate（×value/100）
void CMonsterDrop::_generateRandomItem(unsigned char type, char role,
                                       unsigned char dungeonLv,
                                       unsigned char difficulty,
                                       unsigned char partyPos, int context3c,
                                       int dropRate,
                                       stGenerateResult_t& result, bool& done)
{
    int idx = _getGenItemProbIdx(difficulty);
    if (idx < 0)
        return;

    // 局部拷贝权重行（汇编 7 个连续 mov 至 -0x90..-0x78 区）
    int loRange = m_probsSub[idx].v[0];
    int hiRange = m_probsSub[idx].v[1];
    int weights[5];
    for (int w = 0; w < 5; ++w)
        weights[w] = m_probsSub[idx].v[2 + w];

    unsigned char partyCol = static_cast<unsigned char>(partyPos - 1);

    // 基础概率（flds 链 + fistpl 截断）：
    // dungeonBonus[0][partyPos] × monsterTypeBonus[0][role] × partyBonus[0][partyCol]
    int chance = static_cast<int>(
        *reinterpret_cast<float const*>(&m_dungeonBonus[0][0] + partyPos) *
        *reinterpret_cast<float const*>(&m_monsterTypeBonus[0][0] + role) *
        *reinterpret_cast<float const*>(&m_partyBonus[0][0] + partyCol) *
        static_cast<float>(weights[0]));

    {
        // member_cnt clip：max(context,0)→min(100)；>50 打递减折扣
        context3c = std::min<int>(100, std::max<int>(0, context3c));
        if (context3c > 50)
        {
            float decay = 0.01f + 0.002f * static_cast<float>(context3c - 50);
            chance = static_cast<int>((1.0f - decay) * static_cast<float>(chance));
        }
        chance = (chance * dropRate) / 100;            // imul/sar 除法
    }

    if (!(m_random.generateNumber(0, 10000) < chance))
        return;                                        // setl 不成立跳过物品与金币

    // 金币（getGoldGenTable(type)：+0 基准 / +4 浮动幅度）
    int* gt = getGeneratorCommonData()->getGoldGenTable(type);
    int goldAmt = gt[0] + (gt[0] * GetRandomRange(-gt[1], gt[1])) / 100;

    Inven_Item goldItem;
    goldItem.m_field1 = static_cast<unsigned char>(2); // 字节 +0x01=金币类型
    goldItem.m_addInfo = 0;                            // uStack_1ef 清零

    if (role == 3)
    {
        unsigned short cnt = 0;                        // local_9a
        unsigned short volume = 0;                     // local_9c
        bool bossOk = _getBossGoldDrop(partyCol, cnt, volume);
        if (!bossOk)
        {
            goldItem.set_add_info(std::max<int>(1, goldAmt));
            result.m_dropItems.push_back(goldItem);
        }
        else
        {
            goldAmt = (static_cast<int>(volume) * goldAmt) / 100;
            goldItem.set_add_info(std::max<int>(1, goldAmt));
            for (int i = 0; i < static_cast<int>(cnt); ++i)
                result.m_dropItems.push_back(goldItem);
        }
    }
    else
    {
        goldItem.set_add_info(std::max<int>(1, goldAmt));
        result.m_dropItems.push_back(goldItem);
    }

    if (done)                                          // *(param_9) 非 0 → 结束
        return;

    // ---- 稀有装备主循环 tier 1..4 ----
    for (int tier = 1; tier <= 4; ++tier)
    {
        float marketFactor = 1.0f;                     // local_50
        int altTable[6];                               // local_b4（24B 清零区）
        memset(altTable, 0, sizeof(altTable));

        int* rarityTable = &m_rarityBasis[tier][0];    // this + tier*0x18 + 0x30
        if (tier == 2)
        {
            void* amc = G_CDataManager()->GetAutoMarketContitionsControl();
            float rateF = AM_GetItemDropRate(amc, idx);
            double extra = 0.0;
            if (rateF != 0.0f)
                extra = 1.0 / rateF - 1.0;
            if (AM_ChangeItemRarity(amc, &m_rarityBasis[tier][0], altTable,
                                    tier, extra))
                rarityTable = altTable;
        }

        float weight = static_cast<float>(weights[tier]);
        if (tier == 2)
            weight *= static_cast<float>(m_firstMobWeight) * m_firstMobWeightRate;

        int rollChance = static_cast<int>(
            (*reinterpret_cast<float const*>(&m_partyBonus[tier][0] + partyCol) *
             *reinterpret_cast<float const*>(&m_monsterTypeBonus[tier][0] + role) *
             *reinterpret_cast<float const*>(&m_dungeonBonus[tier][0] + dungeonLv) *
             weight * marketFactor)) * dropRate / 100;

        int randValue = m_random.generateNumber(0, 10000);
        if (!(randValue <= rollChance))
            continue;

        int rarRoll = m_random.generateNumber(1, 1000000);
        int rarity = reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint)
                         ->GetItemRarity(rarityTable, rarRoll, tier);

        G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
            static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
            static_cast<eItemDropType_t>(tier),
            static_cast<ENUM_RARITY>(rarity));

        int itemIdx =
            m_table.chooseItem(static_cast<eItemDropType_t>(tier),
                               static_cast<ENUM_RARITY>(rarity), type);
        if (itemIdx == -1)
            continue;

        if (tier == 2)
        {
            char buf[256];                             // -0xed 起 0x100 日志缓冲
            sprintf(buf,
                    "monster drop rate : item_id(%d), mon_lv(%d), role(%d), "
                    "dungeon_lv(%d), dungeon_diff(%d), member_cnt(%d), "
                    "bourgeois_rate(%d), drop_rate(%d), rand_value(%d), "
                    "drop_value(%d)",
                    itemIdx, static_cast<int>(type), static_cast<int>(role),
                    static_cast<int>(dungeonLv),
                    static_cast<int>(difficulty),
                    static_cast<int>(partyCol), context3c, dropRate, randValue,
                    rollChance);
            Luck_sendItemDropRate(
                reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint), buf);
        }

        CItemList* itemList = G_CDataManager()->m_itemList;
        CItem* item = itemList->find_item(itemIdx);
        if (item == 0)
            continue;

        Inven_Item equip;                              // local_f1 缓冲
        equip.m_addInfo = itemIdx;
        itemList->create_item(static_cast<ENUM_ITEM_CREATE_TYPE>(1), equip, 1);

        if (tier == 2)
        {
            // 非宠物(vtbl+0x14)且非头像(vtbl+0x10)才进入装备升档分支
            if (!item->IsCreatureItem() && !item->IsAvatarItem())
            {
                int grade = item->get_grade();
                int newGrade =
                    Upg_generateEquipGrade(&m_upgrade, type - grade);
                equip.set_add_info(newGrade);
            }
            // ORIG：checkItemUpgradePossible(vtbl+0x5c)==0 且无强化能力 → 设置升级
            if (!item->checkItemUpgradePossible() && !equip.m_amp.hasAbility())
            {
                int grade = item->get_grade();
                ENUM_RARITY ir =
                    static_cast<ENUM_RARITY>(item->get_rarity());
                int upLv = Upg_generateUpgrade(&m_upgrade, ir, type - grade);
                if (upLv > 0)
                    equip.SetUpgrade(static_cast<unsigned char>(upLv));
            }
        }

        int seq = static_cast<int>(result.m_dropItems.size());
        Luck_UseLuckPointM(reinterpret_cast< ::CLuckPoint*>(&result.m_luckPoint),
                           static_cast<int>(equip.m_field1), item->get_rarity(),
                           item->get_grade(), seq);
        result.m_dropItems.push_back(equip);
    }
}

// ---- _generateMonseterSpecificItem — ORIG T 0x08536fce ----
void CMonsterDrop::_generateMonseterSpecificItem(CMonster const* monster,
                                                 char idxType,
                                                 stGenerateResult_t& result)
{
    int roll = m_random.generateNumber(2, 10000);
    int itemIdx = Mon_drop_item_index2(monster, static_cast<unsigned int>(idxType),
                                       roll);
    if (itemIdx != -2)
        pushCreatedItem(itemIdx, result);
}

// ---- generateRandomItem — ORIG T 0x08536da8（虚槽 +0x08）----
void CMonsterDrop::generateRandomItem(stGenerateRefData_t const& ref,
                                      stGenerateResult_t& result)
{
    if (i8_at(ref, 0x35) >= 4)                          // (char)+0x35 ≥4 → 全部返回
        return;

    bool done = false;                                  // local_25

    if (u8_at(ref, 0x40) == 0)                          // hell 旗标低字节为 0
    {
        for (int n = 0; n < i8_at(ref, 0x39); ++n)
        {
            WM_Special_generateRandomItem(
                &m_special,
                u8_at(ref, 0x34), static_cast<char>(i8_at(ref, 0x35)),
                u8_at(ref, 0x36), u8_at(ref, 0x37), u8_at(ref, 0x38),
                *reinterpret_cast<int const*>(
                    reinterpret_cast<char const*>(&ref) + 0x3c),
                u8_at(ref, 0x41), ref.m_dropRate, result, done);
        }
    }

    for (int n = 0; n < i8_at(ref, 0x39); ++n)
    {
        // forceTag 决定本轮首杀权重分量回退：[0]→hunt[0]，[1]→hunt[+4]
        if (ref.m_forceTag.operator[](0))
            memcpy(&m_firstMobWeight, &m_firstMobHunt, sizeof(int));
        else if (ref.m_forceTag.operator[](1))
            memcpy(&m_firstMobWeight,
                   reinterpret_cast<char const*>(&m_firstMobHunt) + 4,
                   sizeof(int));
        m_firstMobWeightRate = ref.m_field1c;           // dword ← ref+0x1c

        _generateRandomItem(u8_at(ref, 0x34),
                            static_cast<char>(i8_at(ref, 0x35)),
                            u8_at(ref, 0x36), u8_at(ref, 0x37), u8_at(ref, 0x38),
                            *reinterpret_cast<int const*>(
                                reinterpret_cast<char const*>(&ref) + 0x3c),
                            ref.m_dropRate, result, done);

        m_firstMobWeight = 1;
        m_firstMobWeightRate = 1.0f;                    // 0x3f800000 位还原
    }
}

// ---- generateSpecificItem — ORIG T 0x08537070（虚槽 +0x0c）----
void CMonsterDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    if (u8_at(ref, 0x41) != 0)
    {
        // hell 旗标非零：地狱派对专用掉落
        WM_Hell_hellPartySpecificItem(&m_hell, ref, result);
        return;
    }

    signed char difficulty = i8_at(ref, 0x3c);
    if (difficulty <= 4)
    {
        CMonster* monster =
            static_cast<CMonster*>(G_CDataManager()->find_monster(u8_at(ref, 0x35)));
        if (monster == 0)
            return;

        STMonsterScript* script =
            static_cast<STMonsterScript*>(monster->getMonsterScript());
        if (!script->m_independentDrops.empty())
            monster->generateIndependentItem(ref, result.m_dropItems);

        Mon_generateDropAvatarItem(monster, u8_at(ref, 0x36),
                                   u8_at(ref, 0x3e), result.m_dropItems);

        script = static_cast<STMonsterScript*>(monster->getMonsterScript());
        if (!script->m_catchItems.empty() && u8_at(ref, 0x45) != 0)
            Mon_generateCatchMonsterItem(monster, result.m_dropItems);

        if (u8_at(ref, 0x43) == 0 && u8_at(ref, 0x44) == 0)
        {
            for (char c = 0; c < i8_at(ref, 0x3d); ++c)
                _generateMonseterSpecificItem(monster, 0, result);
        }
        if (difficulty != 0)
            _generateMonseterSpecificItem(monster, difficulty, result);
        return;
    }

    // 高难度：地狱派对普通 specific 循环 + AI Character 掉落
    for (int n = 0; n < i8_at(ref, 0x3d); ++n)
        WM_Hell_generateSpecificItem(&m_hell, ref, result);

    unsigned int aiIdx = *reinterpret_cast<unsigned int const*>(
        reinterpret_cast<char const*>(&ref) + 0x38);   // dword@+0x38 上/index
    CAICharacter* ai = G_CDataManager()->m_aiCharacterList->get(aiIdx);
    if (ai == 0)
        return;

    ai->generateIndependentItem(ref, result.m_dropItems);
    AI_generateDropAvatarItem(ai, u8_at(ref, 0x36), u8_at(ref, 0x3e),
                              result.m_dropItems);
}

// ---- CMonsterDrop::stFirstMobHuntingWeight_t ctor — ORIG W 0x0853a6ae ----
WongWork::CMonsterDrop::stFirstMobHuntingWeight_t::stFirstMobHuntingWeight_t()
{
    m_field04 = 1;
    m_weight = m_field04;                              // [+4]=1 后拷回 [+0]
}

}  // namespace WongWork
