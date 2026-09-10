bool PacketBuf::get_byte(char& out)
{
    if (is_len_end(1))
    {
        return false;
    }

    out = m_ptr[m_use_size++];
    return true;
