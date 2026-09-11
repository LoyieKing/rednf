// ORIG 0x084b9200（Send_Login.md）：刷新连接秒缓存，构造 Packet_Hades_Login
// （ORIG C1 0x84b9946 W，0xfd2/0x64）：+0x0a acc_id、+0x5f channel_no(字节截断)、
// +0x0e ip[16]（先零化局部缓冲 → GetIP → strcpy）、+0x1e mac strncpy 0x41、
// +0x60 uid；发送后转调 Send_UserStatus()。
void CHades::Send_Login()
{
    m_connSec = GlobalData::s_hades_proxy->GetHadesSocketConnSec();

    Packet_Hades_Login packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_channel = (unsigned char)G_CEnvironment()->get_channel_no();

    char ip[16] = { 0 };
    GetIP(ip);
    strcpy(packet.m_ip, ip);

    strncpy(packet.m_mac, m_user->m_network.get_mac_addr(), 0x41);
    packet.m_uid = m_user->GetUID();

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
    Send_UserStatus();
}