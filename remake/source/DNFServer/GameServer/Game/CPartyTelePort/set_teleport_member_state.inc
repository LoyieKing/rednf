void CPartyTelePort::set_teleport_member_state(int idx, char state)
{
    if (m_party == 0)
    {
        return;
    }
    if (idx < 0)
    {
        return;
    }
    if (idx > 3)
    {
        return;
    }
    // ORIG 0x85bb94x：同上 cMember 成员直访（lea 基址+变址单段、0x12 折叠入 store disp）。
    m_party->GetMember()[idx].m_field12 = state;
