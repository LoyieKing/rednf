void GenerateRandomString(wchar_t* out, int len)
{
    static const unsigned int nAlphabetCount = 26;
    --len;
    for (int i = 0; i < len; ++i)
    {
        int r = std::rand();
        if (r % 3 == 0)
        {
            r >>= 2;
            out[i] = (wchar_t)('A' + r % nAlphabetCount);
        }
        else if (r % 3 == 1)
        {
            r >>= 2;
            out[i] = (wchar_t)('a' + r % nAlphabetCount);
        }
        else
        {
            r >>= 2;
            out[i] = (wchar_t)('0' + r % 10);
        }
    }
    out[len] = L'\0';
