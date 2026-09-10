void CFairMatch::OnJoinRoom(CUser* user)
{
    if (m_field04 != 3)
        m_teams[0].add(user);
