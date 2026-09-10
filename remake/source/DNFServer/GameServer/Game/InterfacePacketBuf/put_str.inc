bool InterfacePacketBuf::put_str(const char* str, int len)
{
    return m_packetBuf->put_str(str, len);
