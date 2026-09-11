int CharString::tokenizeNewLine(const CharString& str,
                                std::vector<CharString>* tokenVector)
{
    const char* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int lastNewLineIdx = -1;
    int tokenCount = 0;
    for (int i = 0; i < strLen; ++i)
    {
        char c = strBuf[i];
        if (c == '\r')
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            if (strBuf[i + 1] == '\n')
                i = i + 1;
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
        else if (c == '\n')
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
        tokenVector->push_back(CharString());
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}