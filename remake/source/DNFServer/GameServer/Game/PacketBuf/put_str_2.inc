bool PacketBuf::put_str(const char* str, int len)
{
    return put_str(m_use_size, str, len);
