std::list<CMission*>* CDataManager::get_kind_mission_list(int idx) const
{
    return &m_pvpMissionSystem->get_kind_mission_list(idx);
}