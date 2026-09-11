void setBit_Byte(unsigned char& value, bool set, int bit)
{
    if (set)
    {
        value = static_cast<unsigned char>(value | (1u << bit));
    }
    else
    {
        value = static_cast<unsigned char>(value & ~static_cast<unsigned char>(1u << bit));
    }
}