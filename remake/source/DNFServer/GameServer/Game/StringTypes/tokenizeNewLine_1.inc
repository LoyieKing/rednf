int WideString::tokenizeNewLine(const WideString& str,
                                std::vector<WideString>* tokenVector)
{
    const wchar_t* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int lastNewLineIdx = -1;
    int tokenCount = 0;
    for (int i = 0; i < strLen; ++i)
    {
        wchar_t c = strBuf[i];
        if (c == 0xd)
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            if (strBuf[i + 1] == 0xa)
                i = i + 1;
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
        else if (c == 0xa)
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
    }
    if (nextStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    else if (strLen - 1 == lastNewLineIdx)
    {
        tokenVector->push_back(WideString());
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}