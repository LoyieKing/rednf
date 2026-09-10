bool PacketBuf::get_int(unsigned long& out)
{
    int value = 0;
    if (get_int(value) == false)
    {
        return false;
    }
    out = (unsigned long)value;
    return true;
