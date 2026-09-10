void CAssaulter::AttachUser(CUser* user, ENUM_ASSAULT_TEAM team)   // ORIG T 0x82e67e6
{
    m_user = user;
    m_team = team;
    _PrepareAssault(user);
