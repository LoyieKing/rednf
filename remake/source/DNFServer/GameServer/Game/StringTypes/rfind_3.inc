int CharString::rfind(int pos, const char* pat, int patLength) const
{
    if (patLength == 0)
        return -1;
    if (pos < 0 || length() - pos < patLength)
        return -1;
    const char* strBuf = c_str();
    for (int thisPos = pos; thisPos > -1; --thisPos)
    {
        if (memcmp(strBuf + thisPos, pat, patLength) == 0)
            return thisPos;
    }
    return -1;
