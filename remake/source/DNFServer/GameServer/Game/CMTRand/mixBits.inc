unsigned long CMTRand::mixBits(const unsigned long& x,
                               const unsigned long& y) const
{
    return hiBit(x) | loBits(y);
