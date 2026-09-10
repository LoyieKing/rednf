bool PacketBuf::put_binary(char* data, int len)
{
    return put_binary(m_use_size, data, len);
