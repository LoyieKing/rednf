void CPartyTelePort::reset_teleport_data()
{
    for (int i = 0; i <= 3; ++i)
    {
        set_teleport_member_state(i, -1);
    }
    m_tp.reset();
    set_teleport_state(-1);
}