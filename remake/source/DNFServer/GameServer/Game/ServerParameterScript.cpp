// ============================================================================
// df_game_r 还原 —— ServerParameterScript（G5 服务器参数脚本）
// 布局依据 ORIG clear（0x0836f38c）/ 析构（0x0837bb42）；
// 逐函数对照 docs/class_func_reports/ServerParameterScript.md。
// ============================================================================

#include "ServerParameterScript.h"

#include <string.h>
#include <algorithm>

#include "DNFLexWrapper.h"

#include <stdio.h>

#include "LogManager.h"

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
        return 1.0f;
    }
    // ORIG 0x08693918：命中 → fildl second; flds 100.0f; fdivrp（second/100.0）
    return (float)it->second / 100.0f;
}

int ServerParameterScript::getRecoverStaminaPrice(int stamina, int price)
{
    int n = std::min((int)m_vec57c.size(), stamina) - 1;
    int v = m_vec57c[n];
    // ORIG 0x0869375a：magic 0xb60b60b7 + sar $0x6 = 除以 90（非 100）
    return ((100 - price) * v) / 90;
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
    if (m_unusableItemFC.find(itemIdx) != m_unusableItemFC.end())
    {
        return false;
    }
    std::map<ENUM_EQUIPMENTTYPE, std::set<unsigned long> >::const_iterator it =
        m_map114.find(equipType);
    if (it != m_map114.end() &&
        it->second.find(itemIdx) != it->second.end())
    {
        return false;
    }
    return true;
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
// ============================================================================
// importServerParameterScript —— ORIG T 0x08a870b7（0x51d4 字节，4616 insn）。
// 按 docs/class_func_reports/_lt_global_gt_/importServerParameterScript.md
// （Ghidra 伪 C + 汇编）逐节 [xxx] 分派完整转写。分支名与写入偏移以 ORIG
// 汇编为准（Ghidra 伪 C 在 [training seal give]/[pvp channel grade penalty]/
// [training first give] 三节错位，已按汇编 0x8a8af11/0x8a8b004/0x8a8b115 修正：
// seal give→m_map4994 2-int 循环、grade penalty→m_map497c 2-int 循环、
// first give→PvPPlayerKey 复杂结构 + m_map49c4 计数）。
// 未命名偏移区（0x34c/0x350、0x5b0..0x5bc、0x5c0 疲劳电池表、0x764..0x768、
// 0x4898/0x493c..0x4950 等）按 ORIG 裸偏移写入，注释标注对应节名。
// ============================================================================
unsigned int g_new_scriptVersion;   // ORIG B 0x0949b298（仅本函数写入）

int GetEquipmentTypeStringToEnum(std::string& type);   // CStackableItem.cpp

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
    bool missed = false;
    char* base = reinterpret_cast<char*>(script);
    while (ScanType(line, true))
    {
        missed = false;
        if (line == "[nation code]")
        {
            ScanInt((bool*)0);
        }
        else if (line == "[script version]")
        {
            script->m_358 = (unsigned int)ScanInt((bool*)0);
            g_new_scriptVersion = script->m_358;
        }
        else if (line == "[version]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_versionBytes[i] = (unsigned char)ScanInt((bool*)0);
            }
        }
        else if (line == "[clear exp bonusrate]")
        {
            script->m_expRate360[0] = ScanFloat((bool*)0);
        }
        else if (line == "[monster exp bonusrate]")
        {
            script->m_expRate360[1] = ScanFloat((bool*)0);
        }
        else if (line == "[party user number exp bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup368[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number exp bonusrate starter server]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup378[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[quest item drop bonus starter server]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_expGroup388[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[dungeon difficulty exp bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_monsterExpTable398[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[clear rank exp bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_expBonusWeight3ac[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[drop prob number]")
        {
            script->m_genRefs0.resize((unsigned int)ScanInt((bool*)0));
        }
        else if (line == "[drop prob]")
        {
            for (int i = 0; i < (int)script->m_genRefs0.size(); ++i)
            {
                sItemGenRef& r = script->m_genRefs0[i];
                r.m_field0 = ScanInt((bool*)0);
                r.m_field4 = ScanInt((bool*)0);
                r.m_field8 = ScanInt((bool*)0);
                r.m_fieldc = ScanInt((bool*)0);
                r.m_dropRate = 1000;
            }
        }
        else if (line == "[dungeon difficulty drop bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_floatPool3cc[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number drop bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_floatPool3e0[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[drop bonusrate of monster kind]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_flagExpRate3f0[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[result reward prob number]")
        {
            script->m_genRefs1.resize((unsigned int)ScanInt((bool*)0));
        }
        else if (line == "[result reward prob]")
        {
            for (int i = 0; i < (int)script->m_genRefs1.size(); ++i)
            {
                sItemGenRef& r = script->m_genRefs1[i];
                r.m_field0 = ScanInt((bool*)0);
                r.m_field4 = ScanInt((bool*)0);
                r.m_field8 = 0;
                r.m_fieldc = ScanInt((bool*)0);
                r.m_dropRate = 1000;
            }
        }
        else if (line == "[dungeon difficulty reward bonusrate]")
        {
            for (int i = 0; i < 5; ++i)
            {
                script->m_floatPool40c[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[party user number reward bonusrate]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_floatPool420[i] = ScanFloat((bool*)0);
            }
        }
        else if (line == "[basis of item dicision]")
        {
            script->m_430 = ScanInt((bool*)0);
        }
        else if (line == "[basis of equipment rarity dicision]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_ints434[i] = ScanInt((bool*)0);
            }
        }
        else if (line == "[basis of stackable rarity dicision]")
        {
            for (int i = 0; i < 4; ++i)
            {
                script->m_ints444[i] = ScanInt((bool*)0);
            }
        }
        else if (line == "[stamina recovery cost]")
        {
            script->m_vec57c.clear();
            while (true)
            {
                ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec57c.push_back(v);
            }
        }
        else if (line == "[increase abuse point]")
        {
            *(int*)(base + 0x5b0) = ScanInt(&ok);
        }
        else if (line == "[decrease abuse point]")
        {
            *(int*)(base + 0x5b4) = ScanInt(&ok);
        }
        else if (line == "[abuse point penalty]")
        {
            *(int*)(base + 0x5b8) = ScanInt(&ok);
        }
        else if (line == "[decrease experience point]")
        {
            *(int*)(base + 0x5bc) = ScanInt(&ok);
        }
        else if (line == "[upgrade with legacy]")
        {
            script->m_508[8] = ScanInt((bool*)0);
            script->m_508[9] = ScanInt((bool*)0);
            script->m_508[10] = ScanInt((bool*)0);
            script->m_508[11] = (script->m_508[9] - script->m_508[8]) * 1000 + 999;
            script->m_508[12] = ScanInt((bool*)0);
            script->m_508[13] = ScanInt((bool*)0);
            script->m_508[14] = ScanInt((bool*)0);
            script->m_508[15] = (script->m_508[13] - script->m_508[12]) * 1000 + 999;
            script->m_508[16] = ScanInt((bool*)0);
            script->m_508[17] = ScanInt((bool*)0);
            script->m_508[18] = ScanInt((bool*)0);
            script->m_508[19] = (script->m_508[17] - script->m_508[16]) * 1000 + 999;
        }
        else if (line == "[notice legacy with rarity]")
        {
            ok = false;
            int v = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptServerParameter.cpp",
                                      "importServerParameterScript", 400,
                                      "importUpgradeTable - invalid notice legacy with rarity tag");
            }
            script->m_558 = v;
        }
        else if (line == "[notice legacy with type]")
        {
            memset(base + 0x55c, 0, 0x1a);
            std::string s;
            while (ScanStr(&s))
            {
                int idx = GetEquipmentTypeStringToEnum(s);
                base[0x55c + idx] = 1;
            }
        }
        else if (line == "[notice legacy with upgrade]")
        {
            ok = false;
            int v = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptServerParameter.cpp",
                                      "importServerParameterScript", 0x1a7,
                                      "importUpgradeTable - invalid notice legacy with upgrade tag");
            }
            script->m_578 = v;
        }
        else if (line == "[collect dungeon statistic server]")
        {
            while (true)
            {
                stServerIndex key;
                ok = false;
                key.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                key.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map0[key] = true;
            }
        }
        else if (line == "[light server no]")
        {
            while (true)
            {
                ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_map24[v] = true;
            }
        }
        else if (line == "[burning fatigue event param]")
        {
            while (true)
            {
                stBurningFatigueParam_t p;
                ok = false;
                p.m_level = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_bonusExpRate = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_refillFatigue = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field0c = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field10 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field14 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[0] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[1] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[2] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field18[3] = (char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_burning48c.push_back(p);
            }
        }
        else if (line == "[fatigue event param]")
        {
            while (true)
            {
                stFatigueEventParam_t p;
                ok = false;
                p.m_field0 = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field8 = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_field9 = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_fielda = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                p.m_fieldb = (unsigned char)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_fatigue4a8.push_back(p);
            }
        }
        else if (line == "[light server notice]")
        {
            while (true)
            {
                stLightServerNotice_t n;
                ok = false;
                n.m_hour = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                n.m_min = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                if (!ScanStr(&n.m_message))
                {
                    return 0;
                }
                script->m_light3c.push_back(n);
            }
        }
        else if (line == "[assault cost]")
        {
            script->m_vec588.clear();
            while (true)
            {
                ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec588.push_back(v);
            }
        }
        else if (line == "[assault channel]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_vec18.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[assault user runaway notice]")
        {
            ok = false;
            if (!ScanStr(&script->m_str594))
            {
                return 0;
            }
        }
        else if (line == "[assault term panalty]")
        {
            script->m_598 = ScanInt((bool*)0);
        }
        else if (line == "[assault term panalty keep up time]")
        {
            script->m_59c = ScanInt((bool*)0);
        }
        else if (line == "[assault level panalty]")
        {
            *(float*)(base + 0x5a0) = ScanFloat((bool*)0);
        }
        else if (line == "[assault chaos point weight]")
        {
            script->m_float5a4.clear();
            while (true)
            {
                ok = false;
                float v = ScanFloat(&ok);
                if (!ok)
                {
                    break;
                }
                if (script->m_float5a4.size() == 0)
                {
                    for (int i = 0; i < 2; ++i)
                    {
                        script->m_float5a4.push_back(v);
                    }
                }
                else
                {
                    for (int i = 0; i < 5; ++i)
                    {
                        script->m_float5a4.push_back(v);
                        if (script->m_float5a4.size() > 99)
                        {
                            break;
                        }
                    }
                }
            }
        }
        else if (line == "[luck point]")
        {
            while (true)
            {
                ok = false;
                int unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                stLuckPoint_t p;
                ok = false;
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                p.m_field1 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_luck48.push_back(p);
            }
        }
        else if (line == "[luck point deduction rare]")
        {
            while (true)
            {
                stLuckPointDeduction_t p;
                ok = false;
                int level = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_levels.reserve(4);
                for (int i = 0; i < 4; ++i)
                {
                    ok = false;
                    int v = ScanInt(&ok);
                    p.m_levels.push_back(v);
                    if (!ok)
                    {
                        return 0;
                    }
                }
                (void)level;
                script->m_luckDed54.push_back(p);
            }
        }
        else if (line == "[luck point deduction unique]")
        {
            while (true)
            {
                stLuckPointDeduction_t p;
                ok = false;
                int level = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                p.m_levels.reserve(4);
                for (int i = 0; i < 4; ++i)
                {
                    ok = false;
                    int v = ScanInt(&ok);
                    p.m_levels.push_back(v);
                    if (!ok)
                    {
                        return 0;
                    }
                }
                (void)level;
                script->m_luckDed60.push_back(p);
            }
        }
        else if (line == "[luck point deduction gap]")
        {
            script->m_vec6c.reserve(4);
            for (int i = 0; i < 3; ++i)
            {
                ok = false;
                int unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                ok = false;
                int v = ScanInt(&ok);
                script->m_vec6c.push_back(v);
                if (!ok)
                {
                    return 0;
                }
            }
            ok = false;
            int unused2 = ScanInt(&ok);
            (void)unused2;
            if (!ok)
            {
                return 0;
            }
            ok = false;
            int unused3 = ScanInt(&ok);
            (void)unused3;
            if (!ok)
            {
                return 0;
            }
        }
        else if (line == "[point per fatigue]")
        {
            script->m_78 = ScanInt((bool*)0);
        }
        else if (line == "[event point per fatigue]")
        {
            script->m_7c = ScanInt((bool*)0);
        }
        else if (line == "[point per play time]")
        {
            script->m_80 = ScanInt((bool*)0);
            script->m_84 = ScanInt((bool*)0);
        }
        else if (line == "[black list help parameter]")
        {
            *(int*)(base + 0x34c) = ScanInt((bool*)0);
            *(int*)(base + 0x350) = ScanInt((bool*)0);
        }
        else if (line == "[unify server prefix]")
        {
            script->m_preventPrefixCount = ScanInt((bool*)0);
            for (int i = 0; i < script->m_preventPrefixCount; ++i)
            {
                ScanStr(script->m_preventPrefixEntries + i * 0x1e, 0x1e);
            }
        }
        else if (line == "[damaged equipment ratio]")
        {
            script->m_49e8 = ScanInt((bool*)0);
        }
        else if (line == "[limit fatigue battery charging amount]")
        {
            *(unsigned short*)(base + 0x5c0) = (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[end level fatigue battery weight]")
        {
            *(float*)(base + 0x5c4) = ScanFloat((bool*)0);
        }
        else if (line == "[character level fatigue battery constance]")
        {
            *(char*)(base + 0x5c8) = (char)ScanInt((bool*)0);
        }
        else if (line == "[limit fatigue battery level]")
        {
            *(char*)(base + 0x5c9) = (char)ScanInt((bool*)0);
        }
        else if (line == "[fatigue battery money amount]")
        {
            *(int*)(base + 0x5cc) = ScanInt((bool*)0);
        }
        else if (line == "[fatigue battery level for money active]")
        {
            *(char*)(base + 0x5d0) = (char)ScanInt((bool*)0);
        }
        else if (line == "[min fatigue battery per buff active]")
        {
            *(unsigned short*)(base + 0x768) = (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[limit grownup buff cnt fatigue battery]")
        {
            int i = 0;
            while (true)
            {
                ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                *(short*)(base + (i + 0x2e8) * 2 + 2) = (short)v;
                ++i;
            }
        }
        else if (line == "[fatigue battery per grownup buff active]")
        {
            *(unsigned short*)(base + 0x764) = (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[grownup buff per fatigue battery active]")
        {
            *(unsigned short*)(base + 0x766) = (unsigned short)ScanInt((bool*)0);
        }
        else if (line == "[pc room party exp bonus rate]")
        {
            ok = false;
            float v = ScanFloat(&ok);
            if (!ok)
            {
                return 0;
            }
            *(float*)(base + 0x76c) = v;
        }
        else if (line == "[escalade tutorial reward]")
        {
            while (true)
            {
                ok = false;
                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map88[key].push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[server message index onoff]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (b == 1)
                {
                    script->m_shared770.m_vecIndex.push_back(a);
                }
            }
        }
        else if (line == "[server message index kind]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_shared770.m_vecMapped.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[server message event kind]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_shared770.m_vecShareKind.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[price average]")
        {
            while (true)
            {
                ok = false;
                int unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vec0.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[lotto cost]")
        {
            script->m_7b8 = ScanInt((bool*)0);
        }
        else if (line == "[jackpot]")
        {
            script->m_7bc = ScanInt((bool*)0);
        }
        else if (line == "[admission increase]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vecc.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[lotto point]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_bloodReward794.m_vec18.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[emblem compound info]")
        {
            while (true)
            {
                ok = false;
                int idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                script->m_emblemCompound[idx].m_cost = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                for (int j = 0; j < 5; ++j)
                {
                    ok = false;
                    script->m_emblemCompound[idx].m_result[j] = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                }
            }
        }
        else if (line == "[recipe upgrade table]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                ok = false;
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    ok = false;
                    script->m_arr868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe amplify table]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                ok = false;
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    ok = false;
                    script->m_arr1868[i][j] = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe upgrade table 2nd]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                ok = false;
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    ok = false;
                    *(int*)(base + (i * 0x20 + j + 0xa18) * 4 + 8) = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[recipe amplify table 2nd]")
        {
            for (int i = 0; i < 0x20; ++i)
            {
                ok = false;
                int unused = ScanInt(&ok);
                (void)unused;
                for (int j = 0; j < 0x20; ++j)
                {
                    ok = false;
                    *(int*)(base + (i * 0x20 + j + 0xe18) * 4 + 8) = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                }
            }
            ScanType(line, true);
        }
        else if (line == "[upper bind cube rate bonus]")
        {
            script->m_49ec = ScanInt(&ok);
        }
        else if (line == "[lower bind cube rate bonus]")
        {
            script->m_49f0 = ScanInt(&ok);
        }
        else if (line == "[full level reward]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map4868[a] = (unsigned int)b;
            }
        }
        else if (line == "[ontime event reward]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                unsigned short b = (unsigned short)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec4880.push_back(std::make_pair(a, (int)b));
            }
        }
        else if (line == "[pvp live event play count]")
        {
            script->m_ac = ScanInt(&ok);
        }
        else if (line == "[pvp live event reward]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_veca0.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[pvp exp penalty revision]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_mapb0[a] = b;
            }
        }
        else if (line == "[unusable equipment info]")
        {
            while (true)
            {
                std::string s;
                ok = false;
                if (!ScanStr(&s))
                {
                    break;
                }
                int type = 0;
                if (s == "relic")
                {
                    type = 2;
                }
                else if (s == "unique")
                {
                    type = 3;
                }
                else if (s == "choronicle")
                {
                    type = 4;
                }
                else if (s == "epic")
                {
                    type = 5;
                }
                else if (s == "rare_avatar")
                {
                    type = 6;
                }
                else if (s == "common")
                {
                    type = 7;
                }
                else if (s == "uncommon")
                {
                    type = 8;
                }
                else if (s == "rare")
                {
                    type = 9;
                }
                else if (s == "rare_creature")
                {
                    type = 10;
                }
                else if (s == "real_rare")
                {
                    type = 1;
                }
                else
                {
                    return 0;
                }
                stUnusableEquipmentInfo info;
                info.m_field0 = type;
                ok = false;
                info.m_field4 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                info.m_field8 = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_unusableEC.push_back(info);
            }
        }
        else if (line == "[daily match count]")
        {
            script->m_f8 = ScanInt(&ok);
        }
        else if (line == "[unusable items]")
        {
            while (true)
            {
                ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_unusableItemFC.insert((unsigned long)v);
            }
        }
        else if (line == "[usable items]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map114[(ENUM_EQUIPMENTTYPE)a].insert((unsigned long)b);
            }
        }
        else if (line == "[dungeon inout time]")
        {
            while (true)
            {
                ok = false;
                int key = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                STDungeonInoutTime t;
                t.m_startHour = ScanInt((bool*)0);
                t.m_endHour = ScanInt((bool*)0);
                t.m_startMin = ScanInt((bool*)0);
                t.m_endMin = ScanInt((bool*)0);
                if (t.m_endHour < t.m_startHour || t.m_endMin < t.m_startMin ||
                    t.m_endHour == 0x18 || t.m_endMin == 0x3c)
                {
                    return 0;
                }
                script->m_dungeonInoutc8[key] = t;
            }
        }
        else if (line == "[member bonus fatigue limit]")
        {
            ok = false;
            script->m_488c = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_4890 = ScanInt(&ok);
        }
        else if (line == "[premiun card cost]")
        {
            *(float*)(base + 0x4894) = ScanFloat((bool*)0);
        }
        else if (line == "[premium card drop]")
        {
            int i = 0;
            while (true)
            {
                ok = false;
                *(int*)(base + (i + 0x489) * 0x10 + 0xc) = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                *(int*)(base + (i + 0x489) * 0x10 + 0x10) = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                *(int*)(base + (i + 0x489) * 0x10 + 0x14) = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                *(int*)(base + (i + 0x489) * 0x10 + 0x18) = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ++i;
                if (9 < i)
                {
                    break;
                }
            }
            *(short*)(base + 0x4898) = (short)i;
        }
        else if (line == "[premium rarity dicision]")
        {
            ok = false;
            *(int*)(base + 0x493c) = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            *(int*)(base + 0x4940) = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            *(int*)(base + 0x4944) = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            *(int*)(base + 0x4948) = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            *(int*)(base + 0x494c) = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            *(int*)(base + 0x4950) = ScanInt(&ok);
        }
        else if (line == "[max clean chatting count]")
        {
            script->m_4960 = ScanInt(&ok);
        }
        else if (line == "[clean chatting count]")
        {
            while (true)
            {
                ok = false;
                int unused = ScanInt(&ok);
                (void)unused;
                if (!ok)
                {
                    break;
                }
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_vec4954.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[clean chatting duplication]")
        {
            script->m_4964 = ScanInt(&ok);
        }
        else if (line == "[max monster drop per map]")
        {
            script->m_4968 = ScanInt(&ok);
        }
        else if (line == "[max monster count per map]")
        {
            script->m_496c = ScanInt(&ok);
        }
        else if (line == "[private training seal gain count]")
        {
            ok = false;
            script->m_schoolMatchSeal[0] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_schoolMatchSeal[1] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_schoolMatchSeal[2] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_schoolMatchSeal[3] = (unsigned char)ScanInt(&ok);
        }
        else if (line == "[relay battle training seal gain count]")
        {
            ok = false;
            script->m_schoolMatchSeal[4] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_schoolMatchSeal[5] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_schoolMatchSeal[6] = (unsigned char)ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_schoolMatchSeal[7] = (unsigned char)ScanInt(&ok);
        }
        else if (line == "[training seal bonus count]")
        {
            *(unsigned short*)(base + 0x4978) = (unsigned short)ScanInt(&ok);
        }
        else if (line == "[training seal give]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map4994[a] = b;
            }
        }
        else if (line == "[pvp channel grade penalty]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_map497c[a] = b;
            }
        }
        else if (line == "[training first give]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int b = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                int c = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                int n = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                std::vector<std::pair<int, int> > vec;
                for (int i = 0; i < n; ++i)
                {
                    ok = false;
                    int x = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    ok = false;
                    int y = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    vec.push_back(std::make_pair(x, y));
                }
                ServerParameterScript::PvPPlayerKey key((unsigned char)a,
                                                       (unsigned char)b,
                                                       (unsigned char)c);
                script->m_pvp49ac[key] = vec;
                std::map<int, int>::iterator it = script->m_map49c4.find(a);
                if (it == script->m_map49c4.end())
                {
                    script->m_map49c4[a] = 1;
                }
                else
                {
                    ++it->second;
                }
            }
        }
        else if (line == "[pvp item give]")
        {
            while (true)
            {
                ok = false;
                int v = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_vec49dc.push_back(v);
            }
        }
        else if (line == "[bring up event]")
        {
            if (ScanType(line, true))
            {
                if (line == "[get eventitem fatigue]")
                {
                    while (true)
                    {
                        ok = false;
                        int v = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                        script->m_eventIdolBringUp454.m_vec8.push_back(v);
                    }
                }
                if (ScanType(line, true))
                {
                    if (line == "[get reward count]")
                    {
                        ok = false;
                        script->m_eventIdolBringUp454.m_field0 = ScanInt(&ok);
                        if (ok)
                        {
                            script->m_eventIdolBringUp454.m_field4 =
                                ScanInt(&ok);
                        }
                    }
                }
            }
            break;
        }
        else if (line == "[protect of upgrade down jpn]")
        {
            while (true)
            {
                ok = false;
                unsigned int key = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stUpgradeItemTicketParam_t p;
                char* pb = reinterpret_cast<char*>(&p);
                ok = false;
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                pb[1] = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                pb[2] = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int f4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (0 < f4)
                {
                    pb[3] = 1;
                }
                script->m_ticket468[key] = p;
            }
        }
        else if (line == "[protect of upgrade down]")
        {
            while (true)
            {
                ok = false;
                unsigned int key = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stUpgradeItemTicketParam_t p;
                char* pb = reinterpret_cast<char*>(&p);
                ok = false;
                p.m_field0 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                pb[1] = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                pb[2] = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                int f4 = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (0 < f4)
                {
                    pb[3] = 1;
                }
                script->m_ticket468[key] = p;
            }
        }
        else if (line == "[weekend bonus fatigue]")
        {
            script->m_weekendBouns4b4.m_bonusFatiguePercent =
                (unsigned short)ScanInt(&ok);
        }
        else if (line == "[weekend bonus exp]")
        {
            script->m_weekendBouns4b4.m_bonusExpPercent = ScanInt(&ok);
        }
        else if (line == "[drop routin by party]")
        {
            for (int i = 0; i < 4; ++i)
            {
                ok = false;
                *(char*)(base + 0x4bc + i) = (char)ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
            }
        }
        else if (line == "[drop upgrade rarity]")
        {
            while (true)
            {
                std::string s;
                ok = false;
                if (!ScanStr(&s))
                {
                    break;
                }
                int rarity = 6;
                if (s == "[common]")
                {
                    rarity = 0;
                }
                else if (s == "[uncommon]")
                {
                    rarity = 1;
                }
                else if (s == "[rare]")
                {
                    rarity = 2;
                }
                else if (s == "[unique]")
                {
                    rarity = 3;
                }
                else if (s == "[epic]")
                {
                    rarity = 4;
                }
                else if (s == "[chronicle]")
                {
                    rarity = 5;
                }
                ItemFarmingData data;
                while (true)
                {
                    ok = false;
                    int itemId = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    ok = false;
                    int weight = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0;
                    }
                    data.m_active += weight;
                    data.m_vecWeight.push_back(std::make_pair(itemId, weight));
                }
                script->m_farming4c0[rarity] = data;
            }
        }
        else if (line == "[drop bonus of dungeon level increase]")
        {
            while (true)
            {
                ok = false;
                int a = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                float b = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_vec4d8.push_back(std::make_pair(a, b));
            }
        }
        else if (line == "[auction register item limit]")
        {
            while (true)
            {
                stAuctionLimitItemParam_t p;
                ok = false;
                p.m_itemId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                p.m_startSec = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                p.m_endSec = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                script->m_auction480.push_back(p);
            }
        }
        else if (line == "[my assault chaos point weight]")
        {
            while (true)
            {
                stAssaultChaosPointWeight w;
                ok = false;
                w.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                w.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                w.m_w0 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                w.m_w1 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_chaosWeight4e4.push_back(w);
            }
        }
        else if (line == "[assault level ghost panalty]")
        {
            while (true)
            {
                stAssaultLevelGhostPanalty g;
                ok = false;
                g.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                g.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                g.m_t0 = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                g.m_t1 = (unsigned int)ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_ghostLevel4f0.push_back(g);
            }
        }
        else if (line == "[assault chaos point ghost panalty]")
        {
            while (true)
            {
                stAssaultChaosPointGhostPanalty g;
                ok = false;
                g.m_min = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                ok = false;
                g.m_max = ScanInt(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                g.m_r0 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                ok = false;
                g.m_r1 = ScanFloat(&ok);
                if (!ok)
                {
                    return 0;
                }
                script->m_ghostChaos4fc.push_back(g);
            }
        }
        else
        {
            missed = true;
        }
        if (missed)
        {
            printf("MISSED SCRIPT TAG [%s] %s\n", path, line.c_str());
        }
    }
    return script->m_558 != -1;
}

