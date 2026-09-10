void CHades::AddHackTypeCnt(unsigned long hackType, unsigned long count,
                            unsigned long param3, unsigned long param4)
{
    needLogin();

    Packet_Hades_AddHackType packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_hackType = (unsigned short)hackType;
    packet.m_count = (unsigned short)count;
    packet.m_field12 = param3;
    packet.m_field16 = param4;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
