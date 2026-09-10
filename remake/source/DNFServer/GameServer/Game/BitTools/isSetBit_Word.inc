bool isSetBit_Word(unsigned short value, int bit)
{
    return static_cast<unsigned short>(1u << bit) & value;
