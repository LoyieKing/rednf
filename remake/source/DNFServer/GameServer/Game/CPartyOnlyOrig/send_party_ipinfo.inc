// ORIG T 0x0859cea2（send_party_ipinfo.md）：header(0,0xb) 广播各成员
// uniqueId/内外网 IP/端口/账号/NAT 类型/MTU 与角色耐力字节，末尾
// finalize(true) 后 send_to_party。
// P157/P48：ORIG 每个使用点都重新走 m_member[i].m_pUser 取用户链（从不
// 缓存局部），getCurCharacR 在 then/else 臂各调用一次；uid/port 低 16 位
// 截断是 caller 侧 movzwl（用 static_cast<unsigned short>，非 & 0xffff）。
void CParty::send_party_ipinfo()
{
    PacketGuard packet;
    packet.put_header(0, 0xb);
    packet.put_byte(get_member_count());
    for (int i = 0; i < 4; i++)
    {
        if (_checkValidUser(i))
        {
            packet.put_short(static_cast<unsigned short>(
                m_member[i].m_pUser->get_unique_id()));
            packet.put_int(m_member[i].m_pUser->m_network.get_inner_ip());
            packet.put_int(m_member[i].m_pUser->m_network.get_ip());
            packet.put_short(static_cast<unsigned short>(
                m_member[i].m_pUser->m_network.get_port()));
            packet.put_int(m_member[i].m_pUser->get_acc_id());
            packet.put_byte(m_member[i].m_pUser->m_network.get_nat_type());
            packet.put_int(m_member[i].m_pUser->m_network.get_mtu());
            if (reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                    ->getCurCharacR() != 0)
            {
                packet.put_byte(
                    reinterpret_cast<CUserCharacInfo*>(m_member[i].m_pUser)
                        ->getCurCharacR()->m_stamina);
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }
    packet.finalize(true);
    send_to_party(packet);
}