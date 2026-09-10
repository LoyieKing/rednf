Packet_CutOff_UDP_Reply_UserInfo::Packet_CutOff_UDP_Reply_UserInfo()
    : PacketHeader(0x3f9, 0xafe), m_count(0), m_channel(255), m_group(0)
{
    memset(m_accIds, 0, sizeof(m_accIds));
