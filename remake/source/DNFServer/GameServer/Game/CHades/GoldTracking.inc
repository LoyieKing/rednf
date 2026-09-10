void CHades::GoldTracking(unsigned char type, unsigned int money, char* ip,
                          unsigned int param4, unsigned int fatigue)
{
    needLogin();

    Packet_Hades_GoldTracking packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_characId = money;
    memset(packet.m_ip, 0, 0x10);
    if (ip != 0)
        strncpy(packet.m_ip, ip, 0x10);
    packet.m_gold = param4;
    packet.m_usedFatigue = (unsigned short)fatigue;
    packet.m_eventType = type;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
