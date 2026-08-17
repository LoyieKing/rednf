// ============================================================================
// df_game_r 还原 —— ServerParameterScript（G5 服务器参数脚本）
// 布局依据 ORIG clear（0x0836f38c）/ 析构（0x0837bb42）；
// 逐函数对照 docs/class_func_reports/ServerParameterScript.md。
// ============================================================================

#include "ServerParameterScript.h"

#include <string.h>

extern "C" bool sub_loadRDARScriptFile(const char* dir, const char* path)
    asm("_Z18loadRDARScriptFilePKcS0_");
extern "C" bool sub_ScanType(std::string& line, bool value)
    asm("_Z8ScanTypeRSsb");
extern "C" int sub_ScanInt(int* out) asm("_Z7ScanIntPi");
extern "C" void sub_SetLightServerFlag(bool flag)
    asm("_Z18SetLightServerFlagb");

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
        m_354[i] = 0;
    }
    m_expBonus360[0] = 1.0f;
    m_expBonus360[1] = 1.0f;
    m_expBonus360[2] = 1.0f;
    m_expBonus360[3] = 1.0f;
    m_370[0] = 1.0f;
    m_370[1] = 1.0f;
    m_370[2] = 1.0f;
    m_370[3] = 1.0f;
    m_380[0] = 1.0f;
    m_380[1] = 1.0f;
    m_380[2] = 1.0f;
    m_380[3] = 1.0f;
    m_390[0] = 1.0f;
    m_390[1] = 1.0f;
    m_390[2] = 1.0f;
    m_390[3] = 1.0f;
    for (int i = 0; i < 5; ++i)
    {
        m_expBonusWeight3a0[i] = 1.0f;
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
    m_vec7a0.clear();
    m_vec7ac.clear();
    m_7b8 = 0;
    m_7bc = 0;
    for (int i = 0; i < 7; ++i)
    {
        m_ghost7c0[i].m_min = -1;
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
        m_4970[i] = 0;
    }
    m_4978 = 0;
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
    m_map0c.clear();
    m_vec18.clear();
    m_map24.clear();
}

float ServerParameterScript::GetExpBonusWeight(eRankBonusIndex idx) const
{
    if (idx < 0 || idx > 4)
    {
        return 0.0f;
    }
    return m_expBonusWeight3a0[idx];
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

bool ServerParameterScript::CheckLightServer(int serverIdx)
{
    std::map<int, bool>::const_iterator it = m_map24.find(serverIdx);
    bool result = false;
    if (it != m_map24.end())
    {
        result = it->second;
        sub_SetLightServerFlag(result);
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
    if (idx >= 0 && idx < (int)m_luck48.size())
    {
        out = m_luck48[idx].m_field0;
        return 1;
    }
    out = 0;
    return 0;
}

int ServerParameterScript::getLuckPointDeductionRate(int idx, int& out)
{
    if (idx >= 0 && idx < (int)m_luckDed54.size())
    {
        out = m_luckDed54[idx].m_field0;
        return 1;
    }
    out = 0;
    return 0;
}

bool ServerParameterScript::LoadScript(const char* path)
{
    return importServerParameterScript(this, path);
}

// ===================== 简单 getter =====================

int ServerParameterScript::GetLottePoint(int idx)
{
    if (m_vec7ac.empty())
    {
        return 0;
    }
    if (idx < 0 || idx >= (int)m_vec7ac.size())
    {
        return 0;
    }
    return m_vec7ac[idx].first;
}

int ServerParameterScript::GetPriceAverage(int idx)
{
    if (m_vec7a0.empty())
    {
        return 0;
    }
    if (idx < 0 || idx >= (int)m_vec7a0.size())
    {
        return 0;
    }
    return m_vec7a0[idx].first;
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
    if (m_vec7a0.empty())
    {
        return 0;
    }
    if (idx < 0 || idx >= (int)m_vec7a0.size())
    {
        return 0;
    }
    return m_vec7a0[idx].second;
}

int ServerParameterScript::GetPvPPenaltyRevision(int grade)
{
    std::map<int, int>::const_iterator it = m_mapb0.find(grade);
    if (it == m_mapb0.end())
    {
        return 0;
    }
    return it->second;
}

int ServerParameterScript::getRecoverStaminaPrice(int idx, int price)
{
    if (idx < 0 || idx >= (int)m_vec588.size())
    {
        return 0;
    }
    return m_vec588[idx] + price;
}

int ServerParameterScript::GetPvPChannelGradePenalty(int grade)
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

bool ServerParameterScript::CheckUnusableEquipmentInfo(int type, int idx)
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

bool ServerParameterScript::CheckUnusableItem(int equipType,
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

int ServerParameterScript::GetTraningSealGive(int idx)
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

bool ServerParameterScript::CheckTraningFirstGive()
{
    return !m_map49c4.empty();
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

bool importServerParameterScript(ServerParameterScript* script,
                                 const char* path)
{
    if (!sub_loadRDARScriptFile("Script/ServerParameterScript", path))
    {
        return false;
    }
    script->clear();
    std::string line;
    int value = 0;
    while (sub_ScanType(line, true))
    {
        if (line == "[lotto]")
        {
            if (sub_ScanInt(&value))
            {
                script->m_7b8 = value;
            }
            if (sub_ScanInt(&value))
            {
                script->m_7bc = value;
            }
        }
        else if (line == "[exp bonus]")
        {
            for (int i = 0; i < 5; ++i)
            {
                if (!sub_ScanInt(&value))
                {
                    break;
                }
                script->m_expBonusWeight3a0[i] = (float)value;
            }
        }
    }
    return true;
}
