// CParty::get_party_realtime_info @ 0x859cbac —— 实时队伍信息（始终返回 1）。
bool CParty::get_party_realtime_info(PacketGuard& packet)
{
    int startIndex = packet.get_index();
    int count = 0;
    packet.put_byte(0);
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            packet.put_short(m_member[i].m_pUser->get_unique_id() & 0xffff);
            // P48：ORIG 0859cc0f/0859ccc4 对 getCurCharacR 双侧各一次独立调用
            //（test %eax;setne 直测 + else 内重新 getCurCharacR 读 +0x75），不缓存。
            // b28d-49 @0x859cc52：ORIG `setne %al; test %al,%al; je <put_byte(0)>`
            // ⇒ 正极性 `if (charac != 0) {stamina} else {0}`（负极性产 sete，ndiff 4）。
            if (m_member[i].m_pUser->getCurCharacR() != 0)
                packet.put_byte(
                    // ORIG 0859cc78 `movzbl 0x75(%eax)` = m_stamina。
                    (int)m_member[i].m_pUser->getCurCharacR()->m_stamina);
            else
                packet.put_byte(0);
            packet.put_byte(isHelpAbuseParty(m_member[i].m_pUser));
            packet.put_byte(i);
            ++count;
        }
    }
    packet.put_byte(startIndex, count);
    return 1;
}