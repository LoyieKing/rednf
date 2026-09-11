void CDataManager::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out)
{
    m_pvpMissionSystem->get_New_DailyMission(out);
}