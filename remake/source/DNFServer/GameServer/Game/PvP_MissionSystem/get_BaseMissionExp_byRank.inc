int PvP_MissionSystem::get_BaseMissionExp_byRank(unsigned short rank) const
{
    if (rank >= m_baseMissionExp.size())
    {
        cMyTrace(
            "unsigned int PvP_MissionSystem::get_BaseMissionExp_byRank(short unsigned int) const",
            0x162, 5)
            ("[@missionSystem] <InvalidRank : %d>", rank);
        return 0;
    }
    return static_cast<int>(m_baseMissionExp[rank]);
