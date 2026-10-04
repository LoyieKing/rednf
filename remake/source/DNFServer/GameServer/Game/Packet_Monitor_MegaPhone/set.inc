void Packet_Monitor_MegaPhone::set(
    unsigned char channel, unsigned short uniqueId, char const* name,
    unsigned char msgLen, char const* msg, unsigned char type)
{
    this->m_channel = channel;
    this->m_uniqueId = uniqueId;
    this->m_type = type;
    strncpy(this->m_name, name, 0x1e);
    strncpy(this->m_msg, msg, 0x100);
    this->m_msgLength = msgLen;
    packetSize = 0x2e + msgLen;
}