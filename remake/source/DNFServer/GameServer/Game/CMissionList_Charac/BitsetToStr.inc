void BitsetToStr(const std::bitset<256>& bitset, std::string& out)
{
    out.resize(0x20);
    for (int i = 0; i < 32; ++i)
    {
        volatile unsigned char byte = 0;
        for (int j = 0; j < 8; ++j)
            byte = (unsigned char)((bitset[i * 8 + j] << j) + byte);
        out[i] = (char)byte;
    }
