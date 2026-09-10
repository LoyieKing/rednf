bool PacketBuf::put_str(int& index, char* str, int len)
{
    if (is_ptr_end(index, len))
    {
        return false;
    }

    for (int i = 0; i < len; i = i + 1)
    {
        if (str[i] == 0)
        {
            break;
        }
        *(char*)(m_ptr + index++) = str[i];
    }
    return true;
