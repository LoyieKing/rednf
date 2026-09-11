// ORIG T 0x0864a97c sz 0x30：PvP 统合频道取 CUser+0x79630（CodeHackCheckStorage
// +0x34 整数覆盖值），否则 CEnvironment+0x378 = m_serverEnvir.m_serverGroup。
int CUser::GetServerGroupToPvP() const
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return m_postChecksumData.m_serverGroup;
    return G_CEnvironment()->m_serverEnvir.m_serverGroup;
}