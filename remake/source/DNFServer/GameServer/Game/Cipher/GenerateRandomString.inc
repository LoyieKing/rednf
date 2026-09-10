void GenerateRandomString(char* out, int len)
{
    static const unsigned int nAlphabetCount = 26;
    --len;
    for (int i = 0; i < len; ++i)
    {
        int r = std::rand();
        if (r % 3 == 0)
        {
            r >>= 2;
            out[i] = (char)('A' + r % nAlphabetCount);
        }
        else if (r % 3 == 1)
        {
            r >>= 2;
            out[i] = (char)('a' + r % nAlphabetCount);
        }
        else
        {
            r >>= 2;
            out[i] = (char)('0' + r % 10);
        }
    }
    out[len] = '\0';
