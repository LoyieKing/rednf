void CParty::setMonsterMoveHack()
{
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            m_member[i].m_pUser->getHackAnalyzer()->addServerHackCnt(
                m_member[i].m_pUser, static_cast<WongWork::ENUM_HACKTYPE>(0xd3),
                1, 0, 0);
        }
    }
