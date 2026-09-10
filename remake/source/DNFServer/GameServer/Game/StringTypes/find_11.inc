int WideString::find(int pos, const wchar_t* pat, int patLength) const
{
    if (patLength == 0)
        return -1;
    if (pos < 0 || length() - pos < patLength)
        return -1;
    const wchar_t* strBuf = c_str();
    int endPos = length() - patLength;
    for (int thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (memcmp(strBuf + thisPos, pat, patLength * sizeof(wchar_t)) == 0)
            return thisPos;
    }
    return -1;
