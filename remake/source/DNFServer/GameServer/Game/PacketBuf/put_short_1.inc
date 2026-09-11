bool PacketBuf::put_short(int value)
{
    return put_short(m_use_size, value);
}