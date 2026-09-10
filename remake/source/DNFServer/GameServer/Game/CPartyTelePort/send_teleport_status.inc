void CPartyTelePort::send_teleport_status(unsigned char status)
{
    if (m_party == 0)
    {
        return;
    }

    PacketGuard pg;

    pg.clear();
    pg.put_header(0, 0x179);
    pg.put_byte((int)get_teleport_village());
    pg.put_byte((int)get_teleport_state());
    for (int i = 0; i <= 3; ++i)
    {
        short uid = -1;
        unsigned char mstate = 0;
        if (m_party->checkValidUser(i) != 0)
        {
            CUser* u = (CUser*)m_party->get_user(i);
            if (u != 0)
            {
                uid = (short)u->get_unique_id();
                mstate = (unsigned char)get_teleport_member_state(i);
            }
        }
        pg.put_short((int)uid);
        pg.put_byte((int)mstate);
    }
    pg.put_byte((int)status);
    pg.finalize(true);
    m_party->send_to_party(pg);
