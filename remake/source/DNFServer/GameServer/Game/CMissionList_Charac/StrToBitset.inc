void StrToBitset(unsigned char* data, std::bitset<256>& bitset)
{
    for (int i = 0; i < 32; ++i)
        for (int j = 0; j < 8; ++j)
            bitset[i * 8 + j] = (data[i] >> j) & 1;
