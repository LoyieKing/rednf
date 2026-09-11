// ORIG 0x084b944a（GoldMinus.md）。Packet_Hades_Gold_Minus（ORIG C1 0x84b99d6 W，
// 0xfdb/0x14）：+0x0a acc_id、+0x0e gold、+0x12 (u16)reason；经 proxy 发送。
void CHades::GoldMinus(unsigned int gold, ENUM_GOLD_CONTROL_REASON reason)
{
    needLogin();

    Packet_Hades_Gold_Minus packet;
    packet.m_accId = m_user->get_acc_id();
    packet.m_gold = gold;
    packet.m_reason = (unsigned short)reason;

    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}