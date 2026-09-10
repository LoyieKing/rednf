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
