// ORIG 0x084b92ea（Send_Logout.md）。Packet_Hades_Logout（ORIG C1 0x84b996a W，
// 0xfd3/0xe）：+0x0a acc_id；经 proxy SendHadesTcpPacket 发送。
void CHades::Send_Logout()
{
    Packet_Hades_Logout packet;
    packet.m_accId = m_user->get_acc_id();
    GlobalData::s_hades_proxy->SendHadesTcpPacket((char*)&packet);
}