bool PacketBuf::put_byte(int value)
{
    return put_byte(m_use_size, value);
