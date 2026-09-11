// ORIG T 0x085ba5d6（send_to_alive_member.md）：存活（user state==5 且
// +0x380 liveFlag 非 0）的成员直发。
void CParty::send_to_alive_member(PacketGuard& packet)
{
    for (int i = 0; i < 4; i++)
    {
        // ORIG O@0x24 `test%al;je` 无 xor（bool 直测跳过 ⇒ 嵌套 if 守卫）。
        if (_checkValidUser(i))
        {
            if (m_member[i].m_pUser->get_state() == 5 &&
                m_battleData.m_arr54[i] != 0)
            {
                m_member[i].m_pUser->Send(packet);
            }
        }
    }
}