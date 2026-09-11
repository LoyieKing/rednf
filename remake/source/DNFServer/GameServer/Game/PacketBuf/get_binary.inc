bool PacketBuf::get_binary(char* out, int size)
{
    if (out == NULL || size == 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x21e,
                              "o_buf == NULL || size == 0");
        return false;
    }

    if (is_len_end(size))
    {
        return false;
    }

    for (int i = 0; i < size; i = i + 1)
    {
        out[i] = m_ptr[m_use_size++];
    }
    return true;
}