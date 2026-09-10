void CDisjointMachine::put_disjoint_machine_info(PacketGuard& p)
{
    p.put_short(static_cast<int>(m_user->get_unique_id() & 0xffff));
    p.put_int(static_cast<int>(m_name.length()));
    p.put_str(m_name.c_str(), static_cast<int>(m_name.length()));
    p.put_byte(static_cast<int>(m_user->getCurCharacVill()));
    p.put_byte(m_user->get_area(false));
    p.put_short(m_field24);
    p.put_short(m_field28);
    p.put_int(m_field1c);
