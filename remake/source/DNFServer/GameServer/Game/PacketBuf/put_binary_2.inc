bool PacketBuf::put_binary(const char* data, int len)
{
    return put_binary(m_use_size, data, len);
