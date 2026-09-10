void PvP_MissionSystem::copy_missionParameter(const MissionParameterScript& param)
{
    m_dailyMissionIndices = param.m_dailyIndices;
    m_field30 = param.m_field0c;
    m_dailyMissionList = param.m_dailyList;
    m_field3c = param.m_field18;
    m_baseMissionExp = param.m_baseExp;
