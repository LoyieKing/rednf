// ============================================================================
// copy_missionParameter（ORIG 0x85e206e）
//   m_dailyMissionIndices(+0x24) = param.m_dailyIndices(+0x00)
//   m_field30(+0x30)             = param.m_field0c
//   m_dailyMissionList(+0x34)    = param.m_dailyList(+0x10)
//   m_field3c(+0x3c)             = param.m_field18
//   m_baseMissionExp(+0x48)      = param.m_baseExp(+0x1c)
// ============================================================================
void PvP_MissionSystem::copy_missionParameter(const MissionParameterScript& param)
{
    m_dailyMissionIndices = param.m_dailyIndices;
    m_field30 = param.m_field0c;
    m_dailyMissionList = param.m_dailyList;
    m_field3c = param.m_field18;
    m_baseMissionExp = param.m_baseExp;
}