int CharString::find(int pos, char pat) const
{
    if (pat == '\0')
        return -1;
    if (pos < 0 || length() <= pos)
        return -1;
    const char* strBuf = c_str();
    int endPos = length() - 1;
    // 注意：ORIG 此处固定用 strBuf[pos] 比较（保留原始行为）。
    for (int thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (strBuf[pos] == pat)
            return thisPos;
    }
    return -1;
