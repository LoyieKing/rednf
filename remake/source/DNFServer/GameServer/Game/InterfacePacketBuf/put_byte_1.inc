bool InterfacePacketBuf::put_byte(int& index, int value)
{
    return m_packetBuf->put_byte(index, value);
