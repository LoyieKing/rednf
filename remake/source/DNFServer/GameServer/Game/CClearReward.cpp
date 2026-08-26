// ============================================================================
// CClearReward.cpp — WongWork::CClearReward 清关奖励生成器实现
// （ORIG 报告 docs/class_func_reports/WongWork__CClearReward/ 13 方法逐段转写）
//
// 覆盖符号（唯一定义点；ctor/loadScript 归 DropLayoutDeps.cpp）：
//   setDropRatio            T 0x085385c8
//   getGoldCardCommission   T 0x085385b2
//   getGoldCardCreateRate   W 0x085bf000
//   getRewardItemRatePerMapMaxCount W 0x085bf018
//   _getGenItemProbIdx      T 0x08538694
//   generateRandomItem      T 0x0853873a（虚槽 +0x08）
//   _generateRandomItem     T 0x085387c0
//   _generateSpecificItem   T 0x08538bf8
//   generateSpecificItem    T 0x08538c84（虚槽 +0x0c）
//   generatePremiumGoldCardItem T 0x08538a92
//   generateGoldCardBlankItem   T 0x08539166
//   generatePcRoomCardBlankItem T 0x0853900c
//   power_war_dungeon_gold_clear_reward T 0x08538e64
// 依赖符号（ORIG T 无所属 TU；一并落于此避免双定义）：
//   WongWork::CItemGenRateTable::chooseItem     T 0x085349a4
//   WongWork::CItemGeneratorMgr::onTryGenerateRandomItem T 0x085342c6
//   ::CLuckPoint::GetItemRarity                 T 0x08550be4
//   ::CLuckPoint::GetLuckPoint                  T 0x085509da
//   ::roundFloat                                W 0x0853abe5
// ============================================================================
#include "CItemGenerator.h"
#include "CItemGeneratorMgr.h"
#include "GameRand.h"

#include "CDataManager.h"
#include "CDungeon.h"
#include "ServerParameterScript.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include <math.h>

namespace WongWork
{

void CClearReward::setDropRatio(float ratio)
{
    for (int grp = 0; grp < 6; ++grp)
    {
        m_workProbs[grp] = m_scriptProbs[grp];
        for (size_t i = 0; i < m_workProbs[grp].size(); ++i)
        {
            // ORIG 085385c8：roundFloat(value * ratio) 回写工作副本。
            m_workProbs[grp][i].v[2] =
                static_cast<int>(roundFloat(static_cast<float>(m_workProbs[grp][i].v[2]) * ratio));
        }
    }
}

int CClearReward::getGoldCardCommission(int n)
{
    return m_goldCardCost[n];
}

float CClearReward::getGoldCardCreateRate()
{
    return m_goldCardCreateRate;
}

int CClearReward::getRewardItemRatePerMapMaxCount(int mapCount) const
{
    if (mapCount > 9)
        return 0;
    return m_rewardRatePerMap[mapCount];
}

int CClearReward::_getGenItemProbIdx(int value, eClearRewardDropProb_t drop)
{
    const std::vector<stGenItemProb_t>& probs = m_workProbs[drop];
    for (size_t i = 0; i < probs.size(); ++i)
    {
        if (value >= probs[i].v[0] && value <= probs[i].v[1])
            return static_cast<int>(i);
    }
    return -1;
}

void CClearReward::generateRandomItem(stGenerateRefData_t const& ref,
                                      stGenerateResult_t& result)
{
    int roll = m_random.generateNumber(1, 10000);
    if (roll < m_dropKindProb[0])
    {
        _generateRandomItem(*reinterpret_cast<stGenerateRefData_t const*>(
                                reinterpret_cast<char const*>(&ref) + 0x34),
                            *reinterpret_cast<int const*>(&ref),
                            result,
                            *reinterpret_cast<float const*>(
                                reinterpret_cast<char const*>(&ref) + 0x38));
    }
    else
    {
        _generateSpecificItem(*reinterpret_cast<stGenerateRefData_t const*>(
                                  reinterpret_cast<char const*>(&ref) + 0x34),
                              result);
    }
}

void CClearReward::_generateRandomItem(stGenerateRefData_t const& refClear,
                                       int param, stGenerateResult_t& result,
                                       float dropRate)
{
    // 第一参 = refClear 首字节（等级基准）；档位 = *(refClear+8) 的 int（drop 组）。
    int dropSlot = *reinterpret_cast<int const*>(
        reinterpret_cast<char const*>(&refClear) + 8);
    int idx = _getGenItemProbIdx(static_cast<unsigned char>(
                                     *reinterpret_cast<char const*>(&refClear)),
                                 static_cast<eClearRewardDropProb_t>(dropSlot));
    if (idx < 0)
        return;

    const stGenItemProb_t& e = m_workProbs[dropSlot][idx];
    float bonusRate = *reinterpret_cast<float const*>(
        reinterpret_cast<char const*>(&refClear) + 4);
    int thr = static_cast<int>(
        roundFloat((1.0f - dropRate) *
                   static_cast<float>(
                       (static_cast<int>(roundFloat(bonusRate * e.v[2])) +
                        m_dungeonDropBonus[reinterpret_cast<unsigned char const*>(&refClear)[1]]) *
                       (param / 100))));
    if (m_random.generateNumber(0, 10000) >= thr)
        return;

    int dropType = 1;   // 初值（未命中即 1）
    int cum = 0;
    int roll = m_random.generateNumber(2, 10000);
    for (int i = 1; i <= 4; ++i)
    {
        cum += m_dropItemTypeProb[i - 1];
        if (roll < cum)
        {
            dropType = i;
            break;
        }
    }
    int rarityRoll = m_random.generateNumber(3, 1000000);
    // ORIG：CLuckPoint::GetItemRarity(this=result+0xc)——全局类与本域 CLuckPoint
    // 前 0xc（vector<CUser*>）布局一致，按 ORIG 同址处理。
    typedef ::CLuckPoint GlobalLuckPoint;
    int rarity = reinterpret_cast<GlobalLuckPoint*>(&result.m_luckPoint)
                     ->GetItemRarity(m_rarityBasis, rarityRoll, dropType);
    G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
        static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
        static_cast<eItemDropType_t>(dropType),
        static_cast<ENUM_RARITY>(rarity));
    int itemIdx = m_table.chooseItem(static_cast<eItemDropType_t>(dropType),
                                     static_cast<ENUM_RARITY>(rarity),
                                     *reinterpret_cast<unsigned char const*>(&refClear));
    if (itemIdx == -1)
        return;
    if (G_CDataManager()->m_itemList->find_item(itemIdx) == 0)
        return;
    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    result.m_dropItems.push_back(inv);
}

void CClearReward::_generateSpecificItem(stGenerateRefData_t const& refClear,
                                         stGenerateResult_t& result)
{
    int roll = m_random.generateNumber(4, 100);
    int itemIdx = refClear.m_dungeon->get_clear_item(roll);
    if (itemIdx == -2)
        return;
    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    result.m_dropItems.push_back(inv);
}

void CClearReward::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    unsigned char const* rc =
        reinterpret_cast<unsigned char const*>(&ref) + 0x34;
    // fA = 队伍加成表(档位-1)；fB = 金币加成表(档位)。
    float fA = m_partyBonus[rc[2] - 1];
    float fB = m_dungeonGoldBonus[rc[1]];
    int rankIdx = ref.m_rankBonusIndex;
    ServerParameterScript* sp =
        reinterpret_cast<ServerParameterScript*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x68);
    float weight = (1.0f + sp->GetExpBonusWeight(
        static_cast<eRankBonusIndex>(rankIdx))) * (fB * fA);
    int* goldTable = m_common->getGoldGenTable(rc[0]);
    int base = (goldTable[0] * 0xaf) / 1000;
    // ORIG 金币输入：ushort@+8(base) + (ushort@+0xe(dungeon 高 16 位)*2 + ushort@+0xa(factor))*2
    unsigned short g0 = *reinterpret_cast<unsigned short const*>(rc + 8);
    unsigned short g1 = *reinterpret_cast<unsigned short const*>(rc + 0xe);
    unsigned short g2 = *reinterpret_cast<unsigned short const*>(rc + 0xa);
    int sum = static_cast<int>(g0 + (g1 * 2 + g2) * 2);
    int baseCount = rc[2];
    int gold = static_cast<int>(roundFloat(static_cast<float>((sum * base) / 2) * weight));
    static const float kGoldDivisor = 2.0f;   // _DAT_08c9a6d8 = 0x40000000 = 2.0f
    for (int i = 0; i < baseCount; ++i)
    {
        float piece = (static_cast<float>(gold) / kGoldDivisor /
                       static_cast<float>(baseCount)) * weight;
        int randSpan = GetRandomRange(-goldTable[1], goldTable[1]);
        int amount = (static_cast<int>(roundFloat(piece)) * (randSpan + 100)) / 100;
        Inven_Item inv;
        inv.m_field1 = static_cast<unsigned char>(2);
        inv.m_addInfo = 0;
        inv.set_add_info(amount);
        result.m_dropItems.push_back(inv);
    }
}

void CClearReward::power_war_dungeon_gold_clear_reward(
    stGenerateRefData_t const& ref, stGenerateResult_t& result, int level)
{
    unsigned char const* rc =
        reinterpret_cast<unsigned char const*>(&ref) + 0x34;
    float fA = m_partyBonus[rc[2] - 1];
    float fB = m_dungeonGoldBonus[rc[1]];
    int rankIdx = ref.m_rankBonusIndex;
    ServerParameterScript* sp =
        reinterpret_cast<ServerParameterScript*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x68);
    float weight = (1.0f + sp->GetExpBonusWeight(
        static_cast<eRankBonusIndex>(rankIdx))) * (fB * fA);
    int* goldTable = m_common->getGoldGenTable(static_cast<unsigned char>(level));
    int base = (goldTable[0] * 0xaf) / 1000;
    unsigned short g0 = *reinterpret_cast<unsigned short const*>(rc + 8);
    unsigned short g1 = *reinterpret_cast<unsigned short const*>(rc + 0xe);
    unsigned short g2 = *reinterpret_cast<unsigned short const*>(rc + 0xa);
    int sum = static_cast<int>(g0 + (g1 * 2 + g2) * 2);
    int gold = static_cast<int>(roundFloat(static_cast<float>((sum * base) / 2) * weight));
    static const float kGoldDivisor = 2.0f;   // _DAT_08c9a6d8 = 2.0f
    float piece = (static_cast<float>(gold) / kGoldDivisor /
                   static_cast<float>(rc[2])) * weight;
    int randSpan = GetRandomRange(-goldTable[1], goldTable[1]);
    int amount = (static_cast<int>(roundFloat(piece)) * (randSpan + 100)) / 100;
    Inven_Item inv;
    inv.m_field1 = static_cast<unsigned char>(2);
    inv.m_addInfo = 0;
    inv.set_add_info(amount);
    result.m_dropItems.push_back(inv);
}

void CClearReward::generatePremiumGoldCardItem(
    stGenerateRefData_t const& ref, stGenerateResult_t& result,
    ENUM_RARITY rarity, unsigned int defaultItem)
{
    int rarityIdx;
    if (static_cast<int>(rarity) < 0)
    {
        int roll = m_random.generateNumber(3, 10000);
        rarityIdx = 0;
        for (int i = 0; i < 6; ++i)
        {
            // dm+0x14+(i+0x1264)*4：六档概率（未被本域具名，按 ORIG 偏移读取）。
            int* prob = reinterpret_cast<int*>(
                reinterpret_cast<char*>(G_CDataManager()) + 0x14 + (i + 0x1264) * 4);
            if (roll <= *prob)
            {
                rarityIdx = i;
                break;
            }
        }
    }
    else
    {
        rarityIdx = static_cast<int>(rarity);
    }
    G_CDataManager()->getItemGenerator()->onTryGenerateRandomItem(
        static_cast<CItemGeneratorMgr::eGenerateType_t>(getGeneratorType()),
        eItemDropType_2, static_cast<ENUM_RARITY>(rarityIdx));
    int itemIdx = m_table.chooseItem(
        eItemDropType_2, static_cast<ENUM_RARITY>(rarityIdx),
        *reinterpret_cast<unsigned char const*>(
            reinterpret_cast<char const*>(&ref) + 0x34));
    if (itemIdx == -1)
        itemIdx = static_cast<int>(defaultItem);
    if (G_CDataManager()->m_itemList->find_item(itemIdx) == 0)
        return;
    Inven_Item inv;
    inv.m_addInfo = itemIdx;
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
    result.m_dropItems.push_back(inv);
}

int CClearReward::generateGoldCardBlankItem(Inven_Item& item)
{
    if (m_goldBlanks.empty())
        return 0;
    int cum = 0;
    int pick = get_rand_int(m_goldCardBlankTotal);
    for (size_t i = 0; i < m_goldBlanks.size(); ++i)
    {
        cum += m_goldBlanks[i].v[0];
        if (pick < cum)
        {
            if (m_goldBlanks[i].v[1] == -1)
                return 0;
            item.m_addInfo = m_goldBlanks[i].v[1];
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), item, m_goldBlanks[i].v[2]);
            return 1;
        }
    }
    return 0;
}

int CClearReward::generatePcRoomCardBlankItem(Inven_Item& item)
{
    if (m_pcRoomBlanks.empty())
        return 0;
    int cum = 0;
    int pick = get_rand_int(m_pcRoomBlankTotal);
    for (size_t i = 0; i < m_pcRoomBlanks.size(); ++i)
    {
        cum += m_pcRoomBlanks[i].v[0];
        if (pick < cum)
        {
            if (m_pcRoomBlanks[i].v[1] == -1)
                return 0;
            item.m_addInfo = m_pcRoomBlanks[i].v[1];
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), item, m_pcRoomBlanks[i].v[2]);
            return 1;
        }
    }
    return 0;
}

// ---- 依赖：CItemGenRateTable::chooseItem（ORIG T 0x085349a4）----
int CItemGenRateTable::chooseItem(eItemDropType_t type, ENUM_RARITY rarity, int level)
{
    if (rarity == RARITY_RARE)
    {
        int itemIdx = static_cast<int>(m_optHandle.choose_item(rarity, level));
        if (itemIdx != -1)
            return itemIdx;
    }
    std::map<unsigned int, int>& slot = m_block.m_slots[type][rarity][level];
    if (slot.empty())
        return -1;
    std::map<unsigned int, int>::iterator it = slot.end();
    --it;
    unsigned long maxKey = it->first;
    unsigned long roll = static_cast<unsigned long>(m_rand->randInt(maxKey));
    std::map<unsigned int, int>::iterator lb = slot.lower_bound(roll);
    if (lb == slot.end())
        --lb;
    return lb->second;
}

// ---- 依赖：CItemGeneratorMgr::onTryGenerateRandomItem（ORIG T 0x085342c6）----
void CItemGeneratorMgr::onTryGenerateRandomItem(eGenerateType_t genType,
                                                eItemDropType_t dropType,
                                                ENUM_RARITY rarity)
{
    // 计数器在对象头统计数组：&this->m_head[0x3c0] 前部（idx*4）。
    int* counter = reinterpret_cast<int*>(reinterpret_cast<char*>(this) +
                                          (rarity + 30 * genType + 6 * dropType) * 4);
    ++(*counter);
}

}  // namespace WongWork

// ============================================================================
// 全局 ::CLuckPoint 幸运点修正（ORIG T 0x085509da / 0x08550be4）
// ============================================================================
// ORIG T 0x08550e50：m_points[idx]（operator[]）得用户序号；越界返 0，
// 否则 m_users[序号]（ORIG 以 int 承载指针值，调用方 m_ownerId 为 int）。
int CLuckPoint::GetUserGainItem(int idx)
{
    if (idx < 0 || static_cast<unsigned int>(idx) >= m_points.size())
    {
        return 0;
    }
    const int userSeq = m_points[idx];
    if (userSeq < 0 || static_cast<unsigned int>(userSeq) >= m_users.size())
    {
        return 0;
    }
    return reinterpret_cast<int>(m_users[userSeq]);
}

int CLuckPoint::GetLuckPoint()
{
    if (m_users.empty())
        return 0;
    int sum = 0;
    for (size_t i = 0; i < m_users.size(); ++i)
    {
        if (m_users[i] != 0)
            sum += reinterpret_cast<CUserCharacInfo*>(m_users[i])->GetCurCharacLuckPoint();
    }
    return sum / static_cast<int>(m_users.size());
}


int CLuckPoint::GetItemRarity(int* table, int roll, int dropType)
{
    int acc[6];
    for (int i = 0; i < 6; ++i)
        acc[i] = 0;

    if (dropType == 2 && !m_users.empty())
    {
        int luck = GetLuckPoint();
        int lp = 0;
        ServerParameterScript* sp = reinterpret_cast<ServerParameterScript*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x68);
        sp->getLuckPoint(luck, lp);
        int neg = -GetTenThousandPercentage(table[2] - table[1], lp);
        int unique = 0;
        int rare = 0;
        for (size_t i = 0; i < m_users.size(); ++i)
        {
            if (m_users[i] != 0)
            {
                CParty* party = reinterpret_cast<CParty*>(m_users[i]->GetParty());
                if (party != 0)
                {
                    unique = party->Get_party_overlapped_drop_ratio_unique();
                    rare = party->Get_party_overlapped_drop_ratio_rare();
                    break;
                }
            }
        }
        if (unique != 0)
        {
            int v = static_cast<int>(
                roundFloat(static_cast<float>(table[3] - table[2]) * 0.01f *
                           static_cast<float>(unique)));
            acc[2] -= v;
            if (table[3] > 999999)
                acc[2] = 0;
        }
        if (rare != 0)
        {
            int v = static_cast<int>(
                roundFloat(static_cast<float>(table[2] - table[1]) * 0.01f *
                           static_cast<float>(rare)));
            neg -= v;
        }
        if (table[1] != 0)
            acc[0] = table[0] * neg / table[1];
        acc[1] = neg;
    }
    for (int i = 0; i <= 5; ++i)
    {
        if (roll < table[i] + acc[i])
            return i;
    }
    return 0;
}

// 全局 ::roundFloat（ORIG W 0x0853abe5 = C round 转 float）
float roundFloat(float v)
{
    return static_cast<float>(round(static_cast<double>(v)));
}
