int CUser::GetServerGroupToPvP() const
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return m_postChecksumData.m_serverGroup;
    return G_CEnvironment()->m_serverEnvir.m_serverGroup;
