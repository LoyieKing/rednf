bool PacketBuf::get_short(unsigned short& out)
{
    short value = 0;
    if (get_short(value) == false)
    {
        return false;
    }
    out = (unsigned short)value;
    return true;
