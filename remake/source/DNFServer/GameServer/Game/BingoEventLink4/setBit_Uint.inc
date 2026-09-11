void setBit_Uint(unsigned int& value, bool set, int bit)
{
    if (set)
    {
        value |= (1u << bit);
    }
    else
    {
        value &= ~(1u << bit);
    }
}