void CAssaulter::DetachUser()
{
    if (m_user->getCurCharacR() != 0)
        m_user->getCurCharacInvenW()->GetCreatureMgrW()->TurnStomach(false);
    m_user = 0;
    m_team = (ENUM_ASSAULT_TEAM)0;
