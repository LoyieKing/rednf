// OnJoinRoom ORIG T 0x8562df6 报告 OnJoinRoom.md
void CFairMatch::OnJoinRoom(CUser* user)
{
    if (m_field04 != 3)
        m_teams[0].add(user);
}