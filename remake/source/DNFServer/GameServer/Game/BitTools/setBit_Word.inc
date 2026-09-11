void setBit_Word(unsigned short& value, bool set, int bit)
{
    if (set)
    {
        value = static_cast<unsigned short>(value | (1u << bit));
    }
    else
    {
        value = static_cast<unsigned short>(value & ~static_cast<unsigned short>(1u << bit));
    }
}