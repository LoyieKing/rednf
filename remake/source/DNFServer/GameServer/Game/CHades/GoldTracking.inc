// ORIG 0x084b949a（GoldTracking.md）：needLogin 后构造 Packet_Hades_GoldTracking
// （ORIG C1 0x84b99fa W，0xfdc/0x29）：+0x0a acc_id、+0x0e money、+0x12 ip[16]
// （显式 memset 0x10，NULL 跳过 strncpy）、+0x22 param4、+0x26 (u16)fatigue、
// +0x28 type；经 proxy 发送。
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
}