bool PacketBuf::get_int(unsigned int& out)
{
    int value = 0;
    if (get_int(value) == false)
    {
        return false;
    }
    out = (unsigned int)value;
    return true;
