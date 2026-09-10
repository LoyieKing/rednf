void CHades::GoldPlus(unsigned int gold, ENUM_GOLD_CONTROL_REASON reason)
{
    needLogin();

    Packet_Hades_Gold_Plus packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_gold = gold;
    packet.m_reason = (unsigned short)reason;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
