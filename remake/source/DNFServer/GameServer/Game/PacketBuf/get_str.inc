bool PacketBuf::get_str(char* out, int maxLen, int len)
{
    if (out == NULL)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x1f8,
                              "o_buf is null");
        return false;
    }
    else if (len < 0 || maxLen < 0)
    {
        return false;
    }
    else if (len >= maxLen)
    {
        return false;
    }
    else
    {
        if (is_len_end(len))
        {
            return false;
        }
        if (len == 0)
        {
            return true;
        }

        int i;
        int c;
        for (i = 0; i < len; i = i + 1)
        {
            c = m_ptr[m_use_size++];
            if (c == 0)
            {
                break;
            }
            out[i] = (char)c;
        }
        out[i] = 0;
        return true;
    }
