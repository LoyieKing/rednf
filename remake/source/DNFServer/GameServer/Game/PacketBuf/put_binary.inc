bool PacketBuf::put_binary(int& index, char* data, int len)
{
    if (data == NULL || len == 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x123,
                              "bin is null or size is zero");
        return false;
    }

    if (is_ptr_end(index, len))
    {
        return false;
    }

    for (int i = 0; i < len; i = i + 1)
    {
        *(char*)(m_ptr + index++) = data[i];
    }
    return true;
