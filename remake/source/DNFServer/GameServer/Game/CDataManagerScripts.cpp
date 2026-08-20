// CDataManager 脚本 POD 的构造/析构实现，独立于通用桩 TU.
#include "CDataManager.h"
#include <cstring>

#define ZERO_POD(T) T::T() { memset(this, 0, sizeof(*this)); } T::~T() {}
ZERO_POD(QuestParameterScript)
ZERO_POD(GuildParameterScript)
ZERO_POD(PowerParameterScript)
AccountCargoScript::AccountCargoScript() : m_reqLevel(0), m_itemIdx(0) {}
AccountCargoScript::~AccountCargoScript() {}
ZERO_POD(PvPSkillTreeParameterScript)
ZERO_POD(IndependentDropParameterScript)
ZERO_POD(channel_script_t)
ZERO_POD(stBroadCastItemScript)
ZERO_POD(eventReward)
ZERO_POD(ConditionLevelChkDungeon)
ZERO_POD(FairPvPStatScript)
ZERO_POD(RegenerationROI)
ZERO_POD(stSeriaBlessingScript)
ZERO_POD(stLevelUpRewardItemScript)
stGrowthCapsuleScript::stGrowthCapsuleScript() { memset(this, 0, sizeof(*this)); }
CLottery_NeedMoney::CLottery_NeedMoney() : m_needMoneyMap() {}
CLottery_NeedMoney::~CLottery_NeedMoney() {}

bool CLottery_NeedMoney::isNeedMoney(unsigned int itemIdx) const
{
    std::map<unsigned int, unsigned int>::const_iterator it = m_needMoneyMap.find(itemIdx);
    return it == m_needMoneyMap.end() ? false : it->second;
}

void CLottery_NeedMoney::add_lottery_needMoney(unsigned int itemIdx, unsigned int cost)
{
    if (m_needMoneyMap.find(itemIdx) == m_needMoneyMap.end())
        m_needMoneyMap.insert(std::make_pair(itemIdx, cost));
}

int CNPCScriptList::find(unsigned int idx) const
{
    __gnu_cxx::hash_map<unsigned int, CNPCScript*>::const_iterator it = m_npcMap.find(idx);
    return it == m_npcMap.end() ? 0 : reinterpret_cast<int>(it->second);
}

namespace WongWork
{
CItemGeneratorMgr::~CItemGeneratorMgr() {}
void CItemGeneratorMgr::setDropRatio(float ratio)
{
    struct Generator { virtual void setDropRatio(float) = 0; };
    for (int i = 0; i < 8; ++i)
        if (m_generators[i])
            static_cast<Generator*>(m_generators[i])->setDropRatio(ratio);
}
void* CItemGeneratorMgr::getGenerator(int type) const
{
    return type >= 0 && type < 8 ? m_generators[type] : 0;
}
}

CItemGeneratorMgr* CDataManager::getItemGenerator()
{
    return m_itemGeneratorMgr;
}

void* CDataManager::get_event_script_mng()
{
    return reinterpret_cast<void*>(m_liveServerInfo.m_eventScriptMng);
}
#undef ZERO_POD
