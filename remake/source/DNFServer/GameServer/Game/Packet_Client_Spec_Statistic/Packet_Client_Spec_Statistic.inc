Packet_Client_Spec_Statistic::Packet_Client_Spec_Statistic()
    : PacketHeader(0x9ce, 0xc06)   // ORIG _ZN12PacketHeaderC1Ett
    , m_flagB(0)                   // movb $0x0,0xb(%eax)
{
}