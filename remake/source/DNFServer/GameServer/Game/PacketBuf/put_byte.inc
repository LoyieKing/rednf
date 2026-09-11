bool PacketBuf::put_byte(int& index, int value)
{
    if (is_ptr_end(index, 1))
    {
        return false;
    }

    *(char*)(m_ptr + index++) = (char)value;
    return true;
}