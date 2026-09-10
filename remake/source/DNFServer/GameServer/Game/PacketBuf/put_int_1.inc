bool PacketBuf::put_int(int value)
{
    return put_int(m_use_size, value);
