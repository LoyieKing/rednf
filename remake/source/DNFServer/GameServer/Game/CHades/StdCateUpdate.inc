// ORIG 0x084b938c。Packet_Hades_StdUpdate（ORIG C1 0x84b9a42 W，0xfd9/0x12）：
//   +0x0a acc_id、+0x0e cate、+0x10 type；经 proxy SendHadesTcpPacket 发送。
void CHades::StdCateUpdate(unsigned short cate, unsigned short type)
{
    needLogin();

    Packet_Hades_StdUpdate packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_stdCate = cate;
    packet.m_fieldC = type;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}