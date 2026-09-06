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
#include "GameMasterCmd.h"   // CGameMasterCharacter::sendServerState（sendItemDropRate 虚发）
#include <math.h>

namespace WongWork
{
// ============================================================================
// stGenerateRefClearReward_t —— generateRandomItem 调用方写入外层 ref+0x34 起
// 的清关奖励上下文视图。ORIG mangled _ZN8WongWork26stGenerateRefClearReward_tE；
// 成员偏移全部来自 _generateRandomItem/_generateSpecificItem 反汇编访问：
//   +0x00 u8 命中基准值 / +0x01 u8 等级档【推断命名，布局实证】
//   +0x04 float 加成率 / +0x08 int dropProb 组 / +0x0c CDungeon*
// sizeof == 0x10（m_dungeon 止，ORIG 访问不超过 +0xf 实证）
// ============================================================================
struct stGenerateRefClearReward_t
{
    unsigned char m_value;      // +0x00
    unsigned char m_level;      // +0x01
    char m_pad02[2];            // +0x02..03
    float m_bonusRate;          // +0x04
    int m_dropProb;             // +0x08（eClearRewardDropProb_t）
    CDungeon* m_dungeon;        // +0x0c
};

// §4.3 裸偏移清零：调用方上下文区在 stGenerateRefData_t 内的起点由**具名字段**
// m_field34（CItemGenerator.h:210）经 offsetof 推导，不再是字面量 0x34；
// 视图尺寸 0x10 由下方断言锁定（本文件上方布局注释：ORIG 访问不超过 +0xf）。
// 取「常量相加」而非「直取字段地址」：ORIG 该处是基址 + 立即数两步物化，
// `&ref.m_field34` 会被 4.4 折成单条 lea，实测 generateRandomItem ndiff 2→14
// 倒退，故保留相加形（常量仍由具名字段推导，无裸偏移字面量）。
static const int kRefClearRewardCtx =
    static_cast<int>(offsetof(stGenerateRefData_t, m_field34));
static_assert(sizeof(stGenerateRefClearReward_t) == 0x10,
              "clear-reward ctx size drift");

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
    // P196：`if (mapCount <= 9) return arr[mapCount]; return 0;` 使数组访问 inline fall-through，
    // ORIG `cmpl $0x9; jg(返0岛)` + 内联 mov。
    if (mapCount <= 9)
        return m_rewardRatePerMap[mapCount];
    return 0;
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
    if (m_random.generateNumber(1, 10000) < m_dropKindProb[0])
    {
        _generateRandomItem(*reinterpret_cast<stGenerateRefClearReward_t const*>(
                                reinterpret_cast<char const*>(&ref) +
                                kRefClearRewardCtx),
                            *reinterpret_cast<int const*>(&ref),
                            result,
                            *reinterpret_cast<float const*>(&ref.m_field04));
    }
    else
    {
        _generateSpecificItem(*reinterpret_cast<stGenerateRefClearReward_t const*>(
                                  reinterpret_cast<char const*>(&ref) +
                                  kRefClearRewardCtx),
                              result);
    }
}
void CClearReward::_generateRandomItem(stGenerateRefClearReward_t const& refClear,
                                       int param, stGenerateResult_t& result,
                                       float dropRate)
{
    // 第一参 = refClear.m_value（命中基准）；档位 = refClear.m_dropProb。
    int idx = _getGenItemProbIdx(
        static_cast<int>(refClear.m_value),
        static_cast<eClearRewardDropProb_t>(refClear.m_dropProb));
    if (idx < 0)
        return;

    const stGenItemProb_t& e = m_workProbs[refClear.m_dropProb][idx];
    // ORIG 两处均 fistpl 截断（fnstcw/fldcw 0xc 截断模式），非 round；
    // 保持 (int) 强转截断语义。
    int thr = static_cast<int>(
        (1.0f - dropRate) *
        static_cast<float>(
            (static_cast<int>(refClear.m_bonusRate * e.v[2]) +
             m_dungeonDropBonus[refClear.m_level]) *
            (param / 100)));

    // ORIG 0x085388ca：generateNumber(0, 10000) 掉落概率判定，roll >= thr 直接退出。
    int dropRoll = m_random.generateNumber(0, 10000);
    if (dropRoll >= thr)
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
                                     refClear.m_value);
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

void CClearReward::_generateSpecificItem(stGenerateRefClearReward_t const& refClear,
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
    // b21 verdict=DIFF（N 残余）：151/169。7=7 调用点全等（G_CDataManager/GetExpBonusWeight/
    //   getGoldGenTable/GetRandomRange/Inven_ItemC1/set_add_info/push_back）。FPU flds/fmuls/
    //   fistpl(fldcw 0xc 截断)时序与 ORIG 全等，金币 sum/base/gold/piece 公式逐位核；rc=+0x34
    //   基址可用性、m_serverParameter(+0x68)、m_common->getGoldGenTable 均核。已把 g0/g1/g2
    //   具名局部内联进 sum 表达式(-3)；register rc 探针无效（Y14 反向）。残余仅 rc[.k] 基址
    //   经栈槽+add vs ORIG 位移寻址、sum/baseCount 占槽(帧 0xb4 vs 0x84+ebx)。语义完成。
    unsigned char const* rc =
        reinterpret_cast<unsigned char const*>(&ref) + 0x34;
    // fA = 队伍加成表(档位-1)；fB = 金币加成表(档位)。
    float fA = m_partyBonus[rc[2] - 1];
    float fB = m_dungeonGoldBonus[rc[1]];
    int rankIdx = ref.m_rankBonusIndex;
    // §4.3 具名化：脚本区 = CDataManager::m_serverParameter（+0x68，
    // CDataManager.h:3551；同 CAssaultMgr.cpp 判例）。
    ServerParameterScript* sp = &G_CDataManager()->m_serverParameter;
    float weight = (1.0f + sp->GetExpBonusWeight(
        static_cast<eRankBonusIndex>(rankIdx))) * (fB * fA);
    int* goldTable = m_common->getGoldGenTable(rc[0]);
    int base = (goldTable[0] * 0xaf) / 1000;
    // ORIG 金币输入：ushort@+8(base) + (ushort@+0xe(dungeon 高 16 位)*2 + ushort@+0xa(factor))*2
    int sum = static_cast<int>(
        *reinterpret_cast<unsigned short const*>(rc + 8) +
        (*reinterpret_cast<unsigned short const*>(rc + 0xe) * 2 +
         *reinterpret_cast<unsigned short const*>(rc + 0xa)) * 2);
    int baseCount = rc[2];
    // ORIG 两处 fistpl 截断（fldcw 0xc），非 round。
    int gold = static_cast<int>(static_cast<float>((sum * base) / 2) * weight);
    static const float kGoldDivisor = 2.0f;   // _DAT_08c9a6d8 = 0x40000000 = 2.0f
    for (int i = 0; i < baseCount; ++i)
    {
        float piece = (static_cast<float>(gold) / kGoldDivisor /
                       static_cast<float>(baseCount)) * weight;
        int randSpan = GetRandomRange(-goldTable[1], goldTable[1]);
        int amount = (static_cast<int>(piece) * (randSpan + 100)) / 100;
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
    // §4.3 具名化：脚本区 = CDataManager::m_serverParameter（+0x68，
    // CDataManager.h:3551；同 CAssaultMgr.cpp 判例）。
    ServerParameterScript* sp = &G_CDataManager()->m_serverParameter;
    float weight = (1.0f + sp->GetExpBonusWeight(
        static_cast<eRankBonusIndex>(rankIdx))) * (fB * fA);
    int* goldTable = m_common->getGoldGenTable(static_cast<unsigned char>(level));
    int base = (goldTable[0] * 0xaf) / 1000;
    unsigned short g0 = *reinterpret_cast<unsigned short const*>(rc + 8);
    unsigned short g1 = *reinterpret_cast<unsigned short const*>(rc + 0xe);
    unsigned short g2 = *reinterpret_cast<unsigned short const*>(rc + 0xa);
    int sum = static_cast<int>(g0 + (g1 * 2 + g2) * 2);
    // ORIG 两处 fistpl 截断，非 round。
    int gold = static_cast<int>(static_cast<float>((sum * base) / 2) * weight);
    static const float kGoldDivisor = 2.0f;   // _DAT_08c9a6d8 = 2.0f
    float piece = (static_cast<float>(gold) / kGoldDivisor /
                   static_cast<float>(rc[2])) * weight;
    int randSpan = GetRandomRange(-goldTable[1], goldTable[1]);
    int amount = (static_cast<int>(piece) * (randSpan + 100)) / 100;
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
        static_cast<unsigned char>(ref.m_field34));   // §4.3 具名化（+0x34）
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
    // ORIG 0x085349a4：try 块覆盖表查询；catch(...) 打 cMyTrace 日志后返回 -1
    // （__cxa_begin_catch → cMyTrace("int WongWork::CItemGenRateTable::chooseItem"
    // "(WongWork::eItemDropType_t, ENUM_RARITY, int)", 0x279, 5) →
    // operator()("[%s][%d].", name, 0x279) → end_catch → return -1）。
    try
    {
        if (rarity == RARITY_RARE)
        {
            int itemIdx = static_cast<int>(m_optHandle.choose_item(rarity, level));
            if (itemIdx != -1)
                return itemIdx;
        }
        // ORIG 槽址 = type*0x7110+rarity*0x12d8+level*24（type 调用方传 1..4），
        // 等价 m_slots[type]（m_slots 无 type0 前缀，直接 5×6×201）。
        std::map<unsigned int, int>& slot = m_slots[type][rarity][level];
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
    catch (...)
    {
        cMyTrace trace("int WongWork::CItemGenRateTable::chooseItem("
                       "WongWork::eItemDropType_t, ENUM_RARITY, int)", 0x279, 5);
        trace("[%s][%d].", "int WongWork::CItemGenRateTable::chooseItem("
              "WongWork::eItemDropType_t, ENUM_RARITY, int)", 0x279);
        return -1;
    }
}

// ---- 依赖：CItemGeneratorMgr::onTryGenerateRandomItem（ORIG T 0x085342c6）----
void CItemGeneratorMgr::onTryGenerateRandomItem(eGenerateType_t genType,
                                                eItemDropType_t dropType,
                                                ENUM_RARITY rarity)
{
    // ORIG T 0x085342c6 索引 = (dropType*6 + genType*30 + rarity)*4 ⇒ 行主序
    // [genType][dropType][rarity]，即权威 CItemGeneratorMgr::m_dropStat（与
    // m_head 同起点的 union 视图；`mov (%esi,%eax,4)`/`mov %esi,(%ebx,%eax,4)`
    // 均 4 字节 ⇒ unsigned int 宽一致，未对齐/越界风险随具名化消除）。
    ++m_dropStat[genType][dropType][rarity];
}

}  // namespace WongWork

// ORIG T 0x08550954：m_users 按幸运点取最大者下标（空表 0）。
unsigned int CLuckPoint::_GetMaxLuckPointUserIndex()
{
    int maxPoint = 0;
    unsigned int best = 0;
    for (unsigned int i = 0; i < m_users.size(); ++i)
    {
        if (maxPoint < m_users[i]->GetCurCharacLuckPoint())       // [0x855098d setg]
        {
            maxPoint = m_users[i]->GetCurCharacLuckPoint();       // [0x85509ae 双 call P48]
            best = i;
        }
    }
    return best;
}

// ORIG T 0x08550b14：最大幸运点用户执行扣点，成功记 m_points[seq]=序号。
bool CLuckPoint::UseLuckPoint(int itemType, int rarity, int grade, int seq)
{
    if (m_users.empty())
    {
        return false;                                   // [0x8550925 guard]
    }
    unsigned int idx = _GetMaxLuckPointUserIndex();
    if ((int)idx < 0 || m_users.size() <= idx)          // [js / ja 两段]
    {
        return false;
    }
    CUser* user = m_users[idx];
    if (user == 0)
    {
        return false;                                   // [0x8550b87]
    }
    if (!UseLuckPoint(user, itemType, rarity, grade))   // [0x8550baf xor $1]
    {
        return false;
    }
    m_points[seq] = static_cast<int>(idx);              // [0x8550bc2 operator[](p4)]
    return true;
}

// ORIG T 0x08550ef0：m_users 中 getCurCharacR 非空者经 GM 虚发文本。
void CLuckPoint::sendItemDropRate(char const* msg)
{
    for (unsigned int i = 0; i < m_users.size(); ++i)   // [seta 无符号循环]
    {
        CUser* user = m_users[i];
        if (user != 0 && user->getCurCharacR() != 0)    // [0x8550f16 两判]
        {
            CGameMasterCharacter* gm = user->GetGameMasterCharacter();
            gm->sendServerState(user, msg);             // [0x8550f50 虚槽+0]
        }
    }
}

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
    // ORIG 0x085509da：divl（无符号除法，int/size_t 提升）；强制 int 会生成 idivl。
    return sum / m_users.size();
}

int CLuckPoint::GetItemRarity(int* table, int roll, int dropType)
{
    int acc[6] = {0};

    if (dropType == 2 && !m_users.empty())
    {
        int luck = GetLuckPoint();
        int lp = 0;
        ServerParameterScript* sp =  // §4.3 具名化（m_serverParameter @+0x68）
            &G_CDataManager()->m_serverParameter;
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
                static_cast<float>(table[3] - table[2]) *
                           static_cast<float>(unique) * 0.01f);
            acc[2] -= v;
            if (table[3] > 999999)
                acc[2] = 0;
        }
        if (rare != 0)
        {
            int v = static_cast<int>(
                static_cast<float>(table[2] - table[1]) *
                           static_cast<float>(rare) * 0.01f);
            neg -= v;
        }
        if (table[1] != 0)
            acc[0] = table[0] * neg / table[1];
        acc[1] = neg;
    }
    int ret = 0;
    for (int i = 0; i <= 5; ++i)
    {
        if (roll <= table[i] + acc[i])
        {
            ret = i;
            break;
        }
    }
    return ret;
}

// 全局 ::CLuckPoint ctor —— ORIG T 0x08550884 含 m_users.clear()。
CLuckPoint::CLuckPoint()
{
    m_users.clear();
}

// 全局 ::roundFloat（ORIG W 0x0853abe5：call round 后 fnstcw/fldcw(0xc 截断)+fistpl
// 取整为 int，直接 mov %eax 返回——返回类型为 int，调用点均为 call 后直接用 eax）
int roundFloat(float v)
{
    return static_cast<int>(round(static_cast<double>(v)));
}
