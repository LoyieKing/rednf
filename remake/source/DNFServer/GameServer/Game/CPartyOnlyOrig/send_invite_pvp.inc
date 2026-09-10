void CParty::send_invite_pvp(int mapIdx)
{
    PacketGuard packet;
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i))     // ORIG 正向 test/jne+真路径 fall-through（技巧17/19）
        {
            packet.clear();
            packet.put_header(0, 7);
            packet.put_short(mapIdx);
            packet.put_byte(2);
            packet.put_int(m_member[i].m_pUser->get_unique_id() & 0xffff);
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
        }
    }
