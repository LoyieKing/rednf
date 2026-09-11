bool InterfacePacketBuf::put_short(int& index, int value)
{
    return m_packetBuf->put_short(index, value);
}