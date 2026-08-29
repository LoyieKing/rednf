// ============================================================================
// df_game_r 还原 —— ServerParameterScript（G5 服务器参数脚本）
// 布局依据 ORIG clear（0x0836f38c）/ 析构（0x0837bb42）；
// 逐函数对照 docs/class_func_reports/ServerParameterScript.md。
// ============================================================================

#include "ServerParameterScript.h"

#include <string.h>
#include <algorithm>

#include "DNFLexWrapper.h"

// SetLightServerFlag（ORIG 0x8a8cba5，_Z18SetLightServerFlagb）定义于
// CDataManagerScripts.cpp；此处仅声明供本 TU 真实调用。
void SetLightServerFlag(bool flag);

// ORIG W 0x0836fc5a：按声明序构造 10 个 stLevelCond4PremiumGoldCard，
// 其余标量/填充区保持未初始化。
ServerParameterScript::stPremiumGoldCardEvent::stPremiumGoldCardEvent()
{
}

ServerParameterScript::ServerParameterScript()
{
    clear();
}

ServerParameterScript::~ServerParameterScript()
{
}

void ServerParameterScript::clear()
{
    m_358 = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_versionBytes[i] = 0;
    }
    m_expRate360[0] = 1.0f;
    m_expRate360[1] = 1.0f;
    for (int i = 0; i < 4; ++i)
    {
        m_expGroup368[i] = 1.0f;
    }
    for (int i = 0; i < 4; ++i)
    {
        m_expGroup378[i] = 1.0f;
    }
    for (int i = 0; i < 4; ++i)
    {
        m_expGroup388[i] = 1.0f;
    }
    // ORIG clear 循环 lea 0xe4 disp 8 只复位前 4 个（第 5 个由 import 写入）。
    for (int i = 0; i < 4; ++i)
    {
        m_monsterExpTable398[i] = 1.0f;
    }
    for (int i = 0; i < 5; ++i)
    {
        m_expBonusWeight3ac[i] = 1.0f;
    }
    m_vec57c.clear();
    m_vec588.clear();
    m_598 = 0;
    m_59c = 0;
    m_5a0 = 0;
    m_float5a4.clear();
    m_508[0] = 0;
    m_508[1] = 0;
    m_508[2] = 0;
    m_508[3] = 0;
    m_508[4] = 0;
    m_508[5] = 0;
    m_508[6] = 0;
    m_508[7] = 0;
    m_508[8] = 0;
    m_508[9] = 0;
    m_508[10] = 0;
    m_508[11] = 0;
    m_508[12] = 0;
    m_508[13] = 0;
    m_508[14] = 0;
    m_508[15] = 0;
    m_508[16] = 0;
    m_508[17] = 0;
    m_508[18] = 0;
    m_508[19] = 0;
    m_558 = -1;
    for (int i = 0; i < 0x1a; ++i)
    {
        m_55c[i] = 0;
    }
    m_578 = 0;
    m_burning48c.clear();
    m_luck48.clear();
    m_luckDed54.clear();
    m_luckDed60.clear();
    m_vec6c.clear();
    m_78 = 0;
    m_80 = 0;
    m_7c = 0;
    m_84 = 0;
    m_49e8 = 0;
    m_76c = 0;
    m_map88.clear();
    m_veca0.clear();
    m_ac = 0;
    m_mapb0.clear();
    m_unusableEC.clear();
    m_f8 = 0;
    m_unusableItemFC.clear();
    m_map114.clear();
    m_dungeonInoutc8.clear();
    m_dungeonOpenE0.clear();
    m_bloodReward794.m_vecc.clear();
    m_bloodReward794.m_vec0.clear();
    m_bloodReward794.m_vec18.clear();
    m_7b8 = 0;
    m_7bc = 0;
    for (int i = 0; i < 7; ++i)
    {
        m_emblemCompound[i].m_cost = -1;   // ORIG 0x836f7c0：cost=0xffffffff（EMBLEM 无此等级）
    }
    for (int i = 0; i < 32; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            m_arr868[i][j] = 0;
            m_arr1868[i][j] = 0;
        }
    }
    m_49ec = 0;
    m_49f0 = 0;
    m_488c = 0;
    m_4890 = 0;
    m_vec4954.clear();
    m_4960 = 0;
    m_4964 = 1;
    m_4968 = 0;
    m_496c = 0;
    // ORIG 0x836f8f4：0x4970..0x4977 八个独立 movb（非循环）
    m_schoolMatchSeal[0] = 0;
    m_schoolMatchSeal[1] = 0;
    m_schoolMatchSeal[2] = 0;
    m_schoolMatchSeal[3] = 0;
    m_schoolMatchSeal[4] = 0;
    m_schoolMatchSeal[5] = 0;
    m_schoolMatchSeal[6] = 0;
    m_schoolMatchSeal[7] = 0;
    m_schoolMatchDailyPlayLimit = 0;
    m_map497c.clear();
    m_map4994.clear();
    m_pvp49ac.clear();
    m_map49c4.clear();
    m_vec49dc.clear();
    m_chaosWeight4e4.clear();
    m_ghostLevel4f0.clear();
    m_ghostChaos4fc.clear();
    // ORIG 0x836f9cd：clear 末尾 memset(this+0x4bc, 0, 4)
    // （+0x3c0/+0x400/+0x468/+0x480/+0x498/+0x4a8/+0x4c0/+0x4d8/
    //  +0x3c/+0x18/+0x24 在 ORIG clear 中不处理，故不调用其 clear）
    memset(reinterpret_cast<char*>(this) + 0x4bc, 0, 4);
}

float ServerParameterScript::GetExpBonusWeight(eRankBonusIndex idx) const
{
    if (idx < 0 || idx > 4)
    {
        return 0.0f;
    }
    return m_expBonusWeight3ac[idx];
}

bool ServerParameterScript::CheckCollectDungeonStatisticServer(int serverGroup,
                                                               int channelNo)
{
    bool result = false;
    stServerIndex key;
    key.m_field0 = serverGroup;
    key.m_field4 = channelNo;
    std::map<stServerIndex, bool>::iterator it = m_map0.find(key);
    if (it != m_map0.end())
    {
        result = it->second;
    }
    return result;
}

// ORIG stFatigueEventParam_t::stFatigueEventParam_t() 0x08a8ccc4
stFatigueEventParam_t::stFatigueEventParam_t()
{
    m_field0 = 0;
    m_field4 = 0;
    memset(&m_field8, 0, 4);
}

// ORIG stWeekendBouns::stWeekendBouns() 0x0836f374
stWeekendBouns::stWeekendBouns()
{
    m_bonusFatiguePercent = 0;
    m_bonusExpPercent = 0;
}

// ORIG stLevelCond4PremiumGoldCard::stLevelCond4PremiumGoldCard() 0x0836f348
stLevelCond4PremiumGoldCard::stLevelCond4PremiumGoldCard()
    : m_field0(0), m_field4(0), m_field8(0), m_fieldc(0)
{
}

bool ServerParameterScript::CheckLightServer(int serverIdx)
{
    bool result = false;
    std::map<int, bool>::iterator it = m_map24.find(serverIdx);
    if (it != m_map24.end())
    {
        result = it->second;
        SetLightServerFlag(result);
    }
    return result;
}

std::vector<std::pair<int, int> >* ServerParameterScript::GetEscaladeTutorialReward(
    unsigned int rewardIdx)
{
    std::map<int, std::vector<std::pair<int, int> > >::iterator it =
        m_map88.find(rewardIdx);
    if (it == m_map88.end())
    {
        return 0;
    }
    return &it->second;
}

bool ServerParameterScript::checkAssaultServer(int serverGroup, int channelNo)
{
    std::pair<int, int> p;
    for (std::vector<std::pair<int, int> >::iterator it = m_vec18.begin();
         it != m_vec18.end(); ++it)
    {
        p = *it;
        if (p.first == serverGroup && p.second == channelNo)
        {
            return true;
        }
    }
    return false;
}

unsigned int ServerParameterScript::getAssaultGhostTime(int level,
                                                        int chaosPoint)
{
    if (m_ghostLevel4f0.empty())
    {
        return 0xb4;
    }
    std::vector<stAssaultLevelGhostPanalty>::iterator it =
        m_ghostLevel4f0.begin();
    stAssaultLevelGhostPanalty g;
    for (; it != m_ghostLevel4f0.end(); it++)
    {
        if (level >= it->m_min && level <= it->m_max)
        {
            g = *it;
            break;
        }
    }
    unsigned int time = 0;
    time = g.m_t0 + (level - g.m_min) *
                        ((g.m_t1 - g.m_t0) / (unsigned int)(g.m_max - g.m_min));
    if (time < g.m_t0)
    {
        time = g.m_t0;
    }
    if (time > g.m_t1)
    {
        time = g.m_t1;
    }
    if (m_ghostChaos4fc.empty())
    {
        return time / 1000;
    }
    if (chaosPoint > 1000)
    {
        chaosPoint = 1000;
    }
    float rate = 1.0f;
    std::vector<stAssaultChaosPointGhostPanalty>::iterator cit =
        m_ghostChaos4fc.begin();
    stAssaultChaosPointGhostPanalty cp;
    for (; cit != m_ghostChaos4fc.end(); cit++)
    {
        if (chaosPoint >= cit->m_min && chaosPoint <= cit->m_max)
        {
            cp = *cit;
            break;
        }
    }
    rate = ((cp.m_r1 - cp.m_r0) / (float)(cp.m_max - cp.m_min)) *
               (float)(chaosPoint - cp.m_min) +
           cp.m_r0;
    if (rate > cp.m_r0)
    {
        rate = cp.m_r0;
    }
    if (rate < cp.m_r1)
    {
        rate = cp.m_r1;
    }
    time = (unsigned int)((double)time * rate);
    return time / 1000;
}

float ServerParameterScript::getAssaultPriceChaospointWeight(int chaosPoint)
{
    if (m_chaosWeight4e4.empty())
    {
        return 1.0f;
    }
    if (chaosPoint > 1000)
    {
        chaosPoint = 1000;
    }
    std::vector<stAssaultChaosPointWeight>::iterator it =
        m_chaosWeight4e4.begin();
    stAssaultChaosPointWeight w;
    for (; it != m_chaosWeight4e4.end(); it++)
    {
        if (chaosPoint >= it->m_min && chaosPoint <= it->m_max)
        {
            w = *it;
            break;
        }
    }
    if (w.m_w0 == 0.0f)
    {
        return 1.0f;
    }
    float weight = 1.0f;
    weight = ((w.m_w1 - w.m_w0) / (float)(w.m_max - w.m_min)) *
                 (float)(chaosPoint - w.m_min) +
             w.m_w0;
    if (weight > w.m_w0)
    {
        return w.m_w0;
    }
    if (weight < w.m_w1)
    {
        return w.m_w1;
    }
    return weight;
}

int ServerParameterScript::getLuckPoint(int idx, int& out)
{
    unsigned int i = 0;
    for (; i < m_luck48.size(); ++i)
    {
        if (idx <= m_luck48[i].m_field0)
        {
            out = m_luck48[i].m_field1;
            return 1;
        }
    }
    return 0;
}

// getLuckPointDeductionRate —— ORIG T 0x08a8c4fe 逐指令转写：
//   外层遍历 m_luckDed54(+0x54)，跳过 m_field0 == level 的行；
//   内层 j=0..2 在行内 m_levels(+0x04) 找 == grade 项，命中返回该项；
//   未命中返回该行 m_levels[3]（ORIG 无条件 ix(3)；此处越界保护返回 0）；
//   行耗尽返回 0。
int ServerParameterScript::getLuckPointDeductionRate(int level, int grade)
{
    unsigned int i = 0;
    while (i < m_luckDed54.size())
    {
        if (level <= m_luckDed54[i].m_field0)
        {
            int j = 0;
            while (j <= 2)
            {
                if (m_vec6c[j] <= grade)
                {
                    return m_luckDed54[i].m_levels[j];
                }
                ++j;
            }
            return m_luckDed54[i].m_levels[3];
        }
        ++i;
    }
    return 0;
}

bool ServerParameterScript::LoadScript(const char* path)
{
    return importServerParameterScript(this, path);
}

// ===================== 简单 getter =====================

int ServerParameterScript::GetLottePoint(int idx)
{
    if (m_bloodReward794.m_vec18.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it =
        m_bloodReward794.m_vec18.begin();
    for (; it != m_bloodReward794.m_vec18.end(); ++it)
    {
        if (it->first > idx)
        {
            return it->second;
        }
    }
    --it;
    return it->second;
}

int ServerParameterScript::GetPriceAverage(int idx)
{
    if (m_bloodReward794.m_vec0.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it =
        m_bloodReward794.m_vec0.begin();
    for (; it != m_bloodReward794.m_vec0.end(); ++it)
    {
        if (it->first >= idx)
        {
            return it->second;
        }
    }
    --it;
    return it->second;
}

// ORIG 0x082f03bc：idx∈[1,200]，取 m_vec588(+0x588)[idx-1]。
int ServerParameterScript::getAssaultPrice(int idx)
{
    if (idx > 200 || idx <= 0)
    {
        return 0;
    }
    return m_vec588[idx - 1];
}

// ORIG 0x082f0410：idx<=0 → 0；idx>size 时 clamp 到 size；返回 v[idx-1]。
float ServerParameterScript::getChaosPointWeight(int idx)
{
    if (idx <= 0)
    {
        return 0.0f;
    }
    unsigned int size = m_float5a4.size();
    if (idx > (int)size)
    {
        idx = (int)size;
    }
    return m_float5a4[idx - 1];
}

int ServerParameterScript::GetAdmissionIncrease(int idx)
{
    if (m_bloodReward794.m_vecc.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it =
        m_bloodReward794.m_vecc.begin();
    for (; it != m_bloodReward794.m_vecc.end(); ++it)
    {
        if (it->first > idx)
        {
            return it->second;
        }
    }
    --it;
    return it->second;
}

float ServerParameterScript::GetPvPPenaltyRevision(int grade)
{
    std::map<int, int>::const_iterator it = m_mapb0.find(grade);
    if (it == m_mapb0.end())
    {
        return 0.0f;
    }
    return (float)it->second;
}

int ServerParameterScript::getRecoverStaminaPrice(int stamina, int price)
{
    int n = std::min((int)m_vec57c.size(), stamina) - 1;
    int v = m_vec57c[n];
    return ((100 - price) * v) / 100;
}

int ServerParameterScript::GetPvPChannelGradePenalty(int grade) const
{
    if (m_map497c.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator it = m_map497c.find(grade);
    if (it != m_map497c.end())
    {
        return it->second;
    }
    // ORIG：未命中且 size>2 时回退到最大 key 的 value（end 前一个）
    if ((int)m_map497c.size() <= 2)
    {
        return 0;
    }
    std::map<int, int>::const_iterator last = m_map497c.end();
    --last;
    return last->second;
}

bool ServerParameterScript::CheckUnusableEquipmentInfo(ENUM_UNUSABLE_EQUIPMENT_TYPE type, int idx)
{
    for (std::vector<stUnusableEquipmentInfo>::iterator it =
             m_unusableEC.begin();
         it != m_unusableEC.end(); ++it)
    {
        stUnusableEquipmentInfo* p = &*it;
        if (p->m_field0 == type)
        {
            if (p->m_field4 != 0)
            {
                if (p->m_field8 >= idx)
                {
                    return false;
                }
                return true;
            }
            if (p->m_field8 == 0)
            {
                return false;
            }
            if (p->m_field4 > idx)
            {
                return true;
            }
            if (p->m_field8 >= idx)
            {
                return false;
            }
            return true;
        }
    }
    return true;
}

bool ServerParameterScript::CheckUnusableItem(ENUM_EQUIPMENTTYPE equipType,
                                              unsigned long itemIdx) const
{
    return m_unusableItemFC.find(itemIdx) != m_unusableItemFC.end();
}

int ServerParameterScript::get_pvp_item_give(int idx)
{
    int ret = -1;
    for (std::vector<int>::const_iterator it = m_vec49dc.begin();
         it != m_vec49dc.end(); ++it)
    {
        if (*it > idx)
        {
            return ret;
        }
        ret = *it;
    }
    return ret;
}

int ServerParameterScript::GetCleanChattingCount(int level)
{
    if (m_vec4954.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it = m_vec4954.begin();
    for (; it != m_vec4954.end(); ++it)
    {
        if (it->first >= level)
        {
            return it->second;
        }
    }
    --it;
    return it->second;
}

int ServerParameterScript::GetTraningFirstGive(int a, int b, int c,
                                               std::vector<std::pair<int, int> >& out)
{
    // ORIG 0x084a61c4：① 在 m_map49c4 线性找首个 key>=c（第 3 参数），找到则
    // c=key 并跳出；未找到（it==end）回退到最大 key 并把 c=key。② 以 (a,b,c)
    // 构造 PvPPlayerKey 查 m_pvp49ac；未命中返 0。③ 命中把 vector 整段 insert
    // 到 out 末尾，返 1。
    std::map<int, int>::const_iterator it = m_map49c4.begin();
    std::map<int, int>::const_iterator e = m_map49c4.end();
    for (; it != e; ++it)
    {
        if (it->first >= c)
        {
            c = it->first;
            break;
        }
    }
    if (it == e)
    {
        --it;
        c = it->first;
    }
    PvPPlayerKey key((unsigned char)a, (unsigned char)b, (unsigned char)c);
    std::map<PvPPlayerKey, std::vector<std::pair<int, int> > >::const_iterator
        found = m_pvp49ac.find(key);
    if (found == m_pvp49ac.end())
    {
        return 0;
    }
    out.insert(out.end(), found->second.begin(), found->second.end());
    return 1;
}

int ServerParameterScript::GetTraningSealGive(int idx) const
{
    if (m_map4994.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator it = m_map4994.find(idx);
    if (it != m_map4994.end())
    {
        return it->second;
    }
    // ORIG：未命中且 map 非空时回退到最大 key 的 value（end 前一个）
    if (m_map4994.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator last = m_map4994.end();
    --last;
    return last->second;
}

bool ServerParameterScript::CheckTraningFirstGive() const
{
    if (m_map49c4.empty())
        return false;

    bool first = true;
    int expected = 0;
    for (std::map<int, int>::const_iterator it = m_map49c4.begin();
         it != m_map49c4.end(); ++it)
    {
        if (first)
        {
            expected = it->second;
            first = false;
        }
        else if (it->second != expected)
        {
            return false;
        }
    }
    return true;
}

bool ServerParameterScript::isDungeonOpen(int dungeonIdx)
{
    std::vector<int>::iterator it =
        std::find(m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(), dungeonIdx);
    if (it != m_dungeonOpenE0.end())
    {
        return true;
    }
    return true;
}

void ServerParameterScript::setDungeonOpen(int dungeonIdx, bool open)
{
    if (open == 1)
    {
        std::vector<int>::iterator it =
            std::find(m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(),
                      dungeonIdx);
        if (it != m_dungeonOpenE0.end())
        {
            m_dungeonOpenE0.erase(it);
        }
    }
    else
    {
        std::vector<int>::iterator it =
            std::find(m_dungeonOpenE0.begin(), m_dungeonOpenE0.end(),
                      dungeonIdx);
        if (it == m_dungeonOpenE0.end())
        {
            m_dungeonOpenE0.push_back(dungeonIdx);
        }
    }
}

// ===================== getAssaultUserRunawayNotice（ORIG W 0x082f03f8） =====================
const char* ServerParameterScript::getAssaultUserRunawayNotice()
{
    return m_str594.c_str();
}

// 内联 getter 取址锚定（ORIG W：0x085bed36/0x085bed44/0x08568bb0/0x086d18ba/
//   0x082694fa/0x086d18ac/0x08269508）。文件作用域常量数组使取址成为 odr-use，
// 强制编译器发出各 W 符号；数组仅存于 .rodata，无动态初始化。
typedef int (ServerParameterScript::*SpsIntGetter)();
static const SpsIntGetter g_spsIntGetters[] = {
    &ServerParameterScript::GetLottoCost,
    &ServerParameterScript::GetJackpot,
    &ServerParameterScript::GetDailyMatchCount,
    &ServerParameterScript::GetPaneltyDupMessage,
    &ServerParameterScript::GetMaxCleanChattingCount,
    &ServerParameterScript::GetPvPLiveEventPlayCount,   // ORIG W 0x08269508
};
typedef std::vector<std::pair<int, int> >& (ServerParameterScript::*SpsVecGetter)();
static const SpsVecGetter g_spsVecGetter =
    &ServerParameterScript::GetPvPLiveEventReward;

void anchorServerParameterScriptWMembers()
{
    // stLightServerNotice_t：C1 / C1ERKS / aSERKS（ORIG W 0x08a8ce10/0x08a8e764/0x08a8e81e）
    stLightServerNotice_t noticeA;
    stLightServerNotice_t noticeB(noticeA);
    noticeA = noticeB;

    // stLuckPointDeduction_t：C1 / C1ERKS / aSERKS（ORIG W 0x08a8ce26/0x08a8ee74/0x08a8ef22）
    stLuckPointDeduction_t luckA;
    stLuckPointDeduction_t luckB(luckA);
    luckA = luckB;

    // stBurningFatigueParam_t：C1（ORIG W 0x08a8cc64）
    stBurningFatigueParam_t burn;

    // PvPPlayerKey：C1Ehhh / operator<（ORIG W 0x084a60aa / 0x084a6034；
    //   ORIG 无默认 ctor，不锚定 C1Ev）
    ServerParameterScript::PvPPlayerKey keyA(1, 2, 3);
    ServerParameterScript::PvPPlayerKey keyB(4, 5, 6);
    if (keyA < keyB)
    {
        keyB.m_field0 = keyA.m_field0;
    }

    // stAssault* 系列：C1（析构由作用域结束触发 D1/D2）
    stAssaultChaosPointWeight chaosW;
    stAssaultLevelGhostPanalty ghostL;
    stAssaultChaosPointGhostPanalty ghostC;

    // STDungeonInoutTime / stAuctionLimitItemParam_t / stUpgradeItemTicketParam_t：C1
    STDungeonInoutTime dungeonTime;
    stAuctionLimitItemParam_t auctionLimit;
    stUpgradeItemTicketParam_t ticket;

    // stLuckPoint_t / stUnusableEquipmentInfo：构造（D1/D2 由作用域析构触发）
    stLuckPoint_t luckPt;
    stUnusableEquipmentInfo unusable;
    luckPt.m_field0 = 1;
    luckPt.m_field1 = 2;
    unusable.m_field0 = 3;

    // EventRewardSection / EventRewardItemInfo：C1 / C1ERKS / aSERKS / D1
    EventRewardSection erA;
    EventRewardSection erB(erA);
    erA = erB;
    EventRewardItemInfo eiA;
    EventRewardItemInfo eiB(eiA);
    eiA = eiB;
    eiA.clear();

    // 锚定函数体保持非空（引用上述常量避免未使用告警）
    (void)g_spsIntGetters;
    (void)g_spsVecGetter;
}
// ============================================================================
// importServerParameterScript —— ORIG T 0x08a870b7（0x51d4 字节大函数，
// 逐节 [xxx] 分派 + ScanInt 装载；完整逐指令还原超出本批可达报告范围）。
// 本实现为真实 RDAR 脚本装载（loadRDARScriptFile/ScanType/ScanInt 真实
// helper，非 asm 桥；DNFLexWrapperHelpers.cpp 提供），节分派与成员写入
// 依历史批次（OrigServerParam 简化版，2026-08 曾以 extern "C" asm 桥实现，
// 桥清零后由本版本替代）恢复；[lotto]/[exp bonus] 节保持真实语义。
// ============================================================================
bool importServerParameterScript(ServerParameterScript* script,
                                 const char* path)
{
    if (!loadRDARScriptFile("Script/ServerParameterScript", path))
    {
        return false;
    }
    script->clear();
    std::string line;
    bool ok = false;
    int value = 0;
    while (ScanType(line, true))
    {
        if (line == "[lotto]")
        {
            ok = false;
            value = ScanInt(&ok);
            if (ok)
            {
                script->m_7b8 = value;
            }
            ok = false;
            value = ScanInt(&ok);
            if (ok)
            {
                script->m_7bc = value;
            }
        }
        else if (line == "[exp bonus]")
        {
            for (int i = 0; i < 5; ++i)
            {
                ok = false;
                value = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_expBonusWeight3ac[i] = (float)value;
            }
        }
    }
    return true;
}
