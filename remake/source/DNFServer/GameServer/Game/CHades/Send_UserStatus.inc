// ORIG 0x084b97fe（Send_UserStatus.md）。Packet_Hades_UserStatus（ORIG C1
// 0x84b9a1e W，0xfdd/0x12）：+0x0a acc_id、+0x0e 状态位域——getSecuType 各位
// 逐次读取：&(1|2|0x20) 任一命中→|1；&8→|2；&0x10→|4。
void CHades::Send_UserStatus()
{
    Packet_Hades_UserStatus packet;
    packet.m_accId = m_user->get_acc_id();

    packet.m_status = 0;

    // ORIG（Send_UserStatus.md）：三路逐次调用 getSecuType()，短路 || 汇聚后 |1。
    if ((m_user->getSecuType() & 1) != 0 ||
        (m_user->getSecuType() & 2) != 0 ||
        (m_user->getSecuType() & 0x20) != 0)
        packet.m_status = packet.m_status | 1;

    if ((m_user->getSecuType() & 8) != 0)
        packet.m_status = packet.m_status | 2;

    if ((m_user->getSecuType() & 0x10) != 0)
        packet.m_status = packet.m_status | 4;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}