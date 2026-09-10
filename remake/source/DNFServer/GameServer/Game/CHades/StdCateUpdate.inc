void CHades::StdCateUpdate(unsigned short cate, unsigned short type)
{
    needLogin();

    Packet_Hades_StdUpdate packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_stdCate = cate;
    packet.m_fieldC = type;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
