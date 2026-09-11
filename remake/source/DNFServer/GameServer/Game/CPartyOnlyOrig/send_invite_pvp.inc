// ORIG T 0x0859d258（send_invite_pvp.md）/ 0x0859d388（send_invite_warroom.md）：
// header(0,7)+短地图索引+邀请类型（pvp=2/warroom=3）+发起者 uid 低 16 位。
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
}