bool CParty::checkDugeonInoutTime(int dungeonIndex)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        if (m_member[i].m_pUser->isGMUser())
            return true;
    }
    return G_CDataManager()->m_serverParameter.isDungeonOpen(dungeonIndex);
