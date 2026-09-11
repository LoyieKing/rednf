// ORIG T 0x82e6812：卸下 creature（TurnStomach false）+ 清 user/team 槽位。
void CAssaulter::DetachUser()
{
    if (m_user->getCurCharacR() != 0)
        m_user->getCurCharacInvenW()->GetCreatureMgrW()->TurnStomach(false);
    m_user = 0;
    m_team = (ENUM_ASSAULT_TEAM)0;
}