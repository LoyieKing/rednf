int WideString::rfind(int pos, wchar_t pat) const
{
    if (pat == 0)
        return -1;
    if (pos < 0 || length() <= pos)
        return -1;
    const wchar_t* strBuf = c_str();
    for (int thisPos = pos; thisPos > -1; --thisPos)
    {
        if (strBuf[thisPos] == pat)
            return thisPos;
    }
    return -1;
