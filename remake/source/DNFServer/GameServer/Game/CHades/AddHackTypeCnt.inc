// ORIG 0x084b9328（AddHackTypeCnt.md）。Packet_Hades_AddHackType（ORIG C1
// 0x84b998e W，0xfd7/0x1a）：+0x0a acc_id、+0x0e (u16)hackType、+0x10 (u16)count、
// +0x12 param3、+0x16 param4；经 proxy 发送。
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
}