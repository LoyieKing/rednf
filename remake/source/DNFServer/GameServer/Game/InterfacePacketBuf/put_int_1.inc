bool InterfacePacketBuf::put_int(int& index, int value)
{
    return m_packetBuf->put_int(index, value);
