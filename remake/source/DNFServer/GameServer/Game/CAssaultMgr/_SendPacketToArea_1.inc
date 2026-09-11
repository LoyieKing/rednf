// ORIG T 0x82e6dce：同门槛；带用户列表的 4 参 send_to_area 版。
//   求值序（报告汇编 +0x25/+0x2e）：m_y 先入 esi、m_x 后入 ebx（右→左），
//   与 send_to_area(int vill, int area, ...) 形参序一致。
void CAssaultPlace::_SendPacketToArea(PacketGuard& packet,
                                      std::vector<CUser*>& users)
{
    if (m_pos.m_x != 0 || m_pos.m_y != 0)
        G_GameWorld()->send_to_area(m_pos.m_x, m_pos.m_y, packet, users);
}