void CHades::Send_Logout()
{
    Packet_Hades_Logout packet;
    packet.m_accId = m_user->get_acc_id();
    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
