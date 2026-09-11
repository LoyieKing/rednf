int CharString::rfind(int pos, char pat) const
{
    if (pat == '\0')
        return -1;
    if (pos < 0 || length() <= pos)
        return -1;
    const char* strBuf = c_str();
    for (int thisPos = pos; thisPos > -1; --thisPos)
    {
        if (strBuf[thisPos] == pat)
            return thisPos;
    }
    return -1;
}