bool InterfacePacketBuf::put_binary(const char* data, int len)
{
    return m_packetBuf->put_binary(data, len);
