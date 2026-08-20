#include "CDataManager.h"

CMission* PvP_MissionSystem::find_mission(int idx)
{
    std::map<int, CMission*>::iterator it = m_missionMap.find(idx);
    return it == m_missionMap.end() ? 0 : it->second;
}

int PvP_MissionSystem::get_WithinMissionIndex(int idx) const
{
    for (std::vector<unsigned int>::size_type i = 0;
         i < m_dailyMissionIndices.size(); ++i)
        if (m_dailyMissionIndices[i] == static_cast<unsigned int>(idx))
            return static_cast<int>(i);
    return -1;
}

std::list<CMission*>& PvP_MissionSystem::get_kind_mission_list(unsigned int idx)
{
    static std::list<CMission*> empty;
    return idx < m_kindMissionList.size() ? m_kindMissionList[idx] : empty;
}

std::bitset<32> PvP_MissionSystem::get_daily_mission_kind() const
{
    return m_dailyMissionKind;
}

int PvP_MissionSystem::get_BaseMissionExp_byRank(unsigned short rank)
{
    return rank < m_baseMissionExp.size() ? static_cast<int>(m_baseMissionExp[rank]) : 0;
}

void PvP_MissionSystem::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out)
{
    out.clear();
    for (std::vector<unsigned int>::size_type i = 0;
         i < m_dailyMissionIndices.size(); ++i) {
        const unsigned int id = m_dailyMissionIndices[i];
        if (find_mission(static_cast<int>(id)))
            out.push_back(std::make_pair(id, static_cast<unsigned int>(i)));
    }
}

void PvP_MissionSystem::sort_list()
{
    for (std::vector<std::list<CMission*> >::iterator it = m_kindMissionList.begin();
         it != m_kindMissionList.end(); ++it)
        it->sort();
}
