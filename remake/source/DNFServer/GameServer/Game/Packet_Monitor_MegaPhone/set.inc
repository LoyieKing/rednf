void Packet_Monitor_MegaPhone::set(
    unsigned char channel, unsigned short uniqueId, char const* name,
    unsigned char msgLen, char const* msg, unsigned char type)
{
    ((Packet_Monitor_MegaPhone_View*)this)->m_channel = channel;
    ((Packet_Monitor_MegaPhone_View*)this)->m_uniqueId = uniqueId;
    ((Packet_Monitor_MegaPhone_View*)this)->m_type = type;
    strncpy(((Packet_Monitor_MegaPhone_View*)this)->m_name, name, 0x1e);
    strncpy(((Packet_Monitor_MegaPhone_View*)this)->m_msg, msg, 0x100);
    ((Packet_Monitor_MegaPhone_View*)this)->m_msgLength = msgLen;
    packetSize = 0x2e + msgLen;
