bool isSetBit_Byte(unsigned char value, int bit)
{
    return static_cast<unsigned char>(1u << bit) & value;
}