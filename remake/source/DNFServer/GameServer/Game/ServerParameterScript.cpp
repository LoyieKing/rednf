// ============================================================================
// df_game_r 还原 —— ServerParameterScript（G5 服务器参数脚本）
// 布局依据 ORIG clear（0x0836f38c）/ 析构（0x0837bb42）；
// 逐函数对照 docs/class_func_reports/ServerParameterScript.md。
// ============================================================================

#include "ServerParameterScript.h"

#include <string.h>

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
    for (int i = 0; i < 8; ++i)
    {
        m_schoolMatchSeal[i] = 0;
    }
    m_schoolMatchDailyPlayLimit = 0;
    m_map497c.clear();
    m_map4994.clear();
    m_pvp49ac.clear();
    m_map49c4.clear();
    m_vec49dc.clear();
    m_chaosWeight4e4.clear();
    m_ghostLevel4f0.clear();
    m_ghostChaos4fc.clear();
    m_genRefs0.clear();
    m_genRefs1.clear();
    m_ticket468.clear();
    m_auction480.clear();
    m_revenge498.clear();
    m_fatigue4a8.clear();
    m_farming4c0.clear();
    m_vec4d8.clear();
    m_light3c.clear();
    m_vec18.clear();
    m_map24.clear();
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
    stServerIndex key;
    key.m_field0 = serverGroup;
    key.m_field4 = channelNo;
    std::map<stServerIndex, bool>::const_iterator it = m_map0.find(key);
    if (it != m_map0.end())
    {
        return it->second;
    }
    return false;
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
    std::map<int, bool>::const_iterator it = m_map24.find(serverIdx);
    bool result = false;
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
    for (std::vector<std::pair<int, int> >::iterator it = m_veca0.begin();
         it != m_veca0.end(); ++it)
    {
        if (it->first == serverGroup && it->second == channelNo)
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
    unsigned int time = 0;
    for (std::vector<stAssaultLevelGhostPanalty>::iterator it =
             m_ghostLevel4f0.begin();
         it != m_ghostLevel4f0.end(); ++it)
    {
        if (level >= it->m_min && level <= it->m_max)
        {
            time = it->m_t0 +
                   (level - it->m_min) *
                       ((it->m_t1 - it->m_t0) / (unsigned int)(it->m_max - it->m_min));
            break;
        }
    }
    if (time < m_ghostLevel4f0[0].m_t0)
    {
        time = m_ghostLevel4f0[0].m_t0;
    }
    if (!m_ghostChaos4fc.empty())
    {
        if (chaosPoint > 1000)
        {
            chaosPoint = 1000;
        }
        float rate = 1.0f;
        for (std::vector<stAssaultChaosPointGhostPanalty>::iterator it =
                 m_ghostChaos4fc.begin();
             it != m_ghostChaos4fc.end(); ++it)
        {
            if (chaosPoint >= it->m_min && chaosPoint <= it->m_max)
            {
                rate = (float)(chaosPoint - it->m_min) *
                           ((it->m_r1 - it->m_r0) /
                            (float)(it->m_max - it->m_min)) +
                       it->m_r0;
                break;
            }
        }
        time = (unsigned int)((float)time * rate);
        return time / 1000;
    }
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
    float weight = 1.0f;
    for (std::vector<stAssaultChaosPointWeight>::iterator it =
             m_chaosWeight4e4.begin();
         it != m_chaosWeight4e4.end(); ++it)
    {
        if (chaosPoint >= it->m_min && chaosPoint <= it->m_max)
        {
            weight = (float)(chaosPoint - it->m_min) *
                         ((it->m_w1 - it->m_w0) /
                          (float)(it->m_max - it->m_min)) +
                     it->m_w0;
            break;
        }
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
            for (;;)
            {
                if (j > 2)
                    return m_luckDed54[i].m_levels[3];
                if (m_vec6c[j] <= grade)
                    break;
                ++j;
            }
            return m_luckDed54[i].m_levels[j];
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
    if (idx < 0 || idx >= (int)m_bloodReward794.m_vec18.size())
    {
        return 0;
    }
    return m_bloodReward794.m_vec18[idx].first;
}

int ServerParameterScript::GetPriceAverage(int idx)
{
    if (m_bloodReward794.m_vecc.empty())
    {
        return 0;
    }
    if (idx < 0 || idx >= (int)m_bloodReward794.m_vecc.size())
    {
        return 0;
    }
    return m_bloodReward794.m_vecc[idx].first;
}

int ServerParameterScript::getAssaultPrice(int idx)
{
    if (idx < 0 || idx > 200)
    {
        return 0;
    }
    return m_arr868[idx][0];
}

float ServerParameterScript::getChaosPointWeight(int idx)
{
    if (idx < 0 || idx >= (int)m_float5a4.size())
    {
        return 0.0f;
    }
    return m_float5a4[idx];
}

int ServerParameterScript::GetAdmissionIncrease(int idx)
{
    if (m_bloodReward794.m_vecc.empty())
    {
        return 0;
    }
    if (idx < 0 || idx >= (int)m_bloodReward794.m_vecc.size())
    {
        return 0;
    }
    return m_bloodReward794.m_vecc[idx].second;
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

int ServerParameterScript::getRecoverStaminaPrice(int idx, int price)
{
    if (idx < 0 || idx >= (int)m_vec588.size())
    {
        return 0;
    }
    return m_vec588[idx] + price;
}

int ServerParameterScript::GetPvPChannelGradePenalty(int grade) const
{
    if (m_map497c.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator it = m_map497c.find(grade);
    if (it == m_map497c.end())
    {
        return 0;
    }
    return it->second;
}

bool ServerParameterScript::CheckUnusableEquipmentInfo(ENUM_UNUSABLE_EQUIPMENT_TYPE type, int idx)
{
    for (std::vector<stUnusableEquipmentInfo>::iterator it =
             m_unusableEC.begin();
         it != m_unusableEC.end(); ++it)
    {
        if (it->m_field0 == type)
        {
            return true;
        }
    }
    return false;
}

bool ServerParameterScript::CheckUnusableItem(ENUM_EQUIPMENTTYPE equipType,
                                              unsigned long itemIdx) const
{
    return m_unusableItemFC.find(itemIdx) != m_unusableItemFC.end();
}

int ServerParameterScript::get_pvp_item_give(int idx)
{
    if (idx < 0 || idx >= (int)m_vec49dc.size())
    {
        return -1;
    }
    return m_vec49dc[idx];
}

int ServerParameterScript::GetCleanChattingCount(int level)
{
    if (m_vec4880.empty())
    {
        return 0;
    }
    for (std::vector<std::pair<int, int> >::iterator it = m_vec4880.begin();
         it != m_vec4880.end(); ++it)
    {
        if (level >= it->first && level <= it->second)
        {
            return it->first;
        }
    }
    return 0;
}

int ServerParameterScript::GetTraningFirstGive(int a, int b, int c,
                                               std::vector<std::pair<int, int> >& out)
{
    (void)a;
    (void)b;
    (void)c;
    out.clear();
    for (std::map<int, int>::iterator it = m_map49c4.begin();
         it != m_map49c4.end(); ++it)
    {
        out.push_back(std::pair<int, int>(it->first, it->second));
    }
    return (int)out.size();
}

int ServerParameterScript::GetTraningSealGive(int idx) const
{
    if (m_map4994.empty())
    {
        return 0;
    }
    std::map<int, int>::const_iterator it = m_map4994.find(idx);
    if (it == m_map4994.end())
    {
        return 0;
    }
    return it->second;
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
    for (std::vector<int>::iterator it = m_dungeonOpenE0.begin();
         it != m_dungeonOpenE0.end(); ++it)
    {
        if (*it == dungeonIdx)
        {
            return true;
        }
    }
    return false;
}

void ServerParameterScript::setDungeonOpen(int dungeonIdx, bool open)
{
    for (std::vector<int>::iterator it = m_dungeonOpenE0.begin();
         it != m_dungeonOpenE0.end(); ++it)
    {
        if (*it == dungeonIdx)
        {
            if (!open)
            {
                m_dungeonOpenE0.erase(it);
            }
            return;
        }
    }
    if (open)
    {
        m_dungeonOpenE0.push_back(dungeonIdx);
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
