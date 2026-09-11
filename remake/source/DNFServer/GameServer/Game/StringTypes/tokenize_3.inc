int WideString::tokenize(const WideString& str, std::vector<WideString>* tokenVector,
                         const wchar_t* dropDelimiters, const wchar_t* returnDelimiters,
                         bool isKeepEmptyToken)
{
    // [b28d-solo-StringTypes] 收敛（ORIG 0x08adbd56）：同 CharString::tokenize，
    // 交换两臂 + 取反条件复现 ORIG 的 `je <远处 d1 分支>` + drop 分支直落块序，
    // 并按 P212 全头部声明复现槽位。
    const wchar_t* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int tokenCount = 0;
    int i;
    wchar_t c;
    const wchar_t* d0;
    const wchar_t* d1;
    int newLen;
    for (i = 0; i < strLen; ++i)
    {
        c = strBuf[i];
        d0 = dropDelimiters;
        while (*d0 != 0 && *d0 != c)
            ++d0;
        if (*d0 != 0)
        {
            newLen = i - nextStartIdx;
            if (newLen == 0)
            {
                if (isKeepEmptyToken)
                {
                    tokenVector->push_back(WideString());
                    tokenCount = tokenCount + 1;
                }
            }
            else
            {
                tokenVector->push_back(mid(str, nextStartIdx, newLen));
                tokenCount = tokenCount + 1;
            }
            nextStartIdx = i + 1;
        }
        else
        {
            d1 = returnDelimiters;
            while (*d1 != 0 && *d1 != c)
                ++d1;
            if (*d1 != 0)
            {
                int newLen = i - nextStartIdx;
                if (newLen == 0)
                {
                    if (isKeepEmptyToken)
                    {
                        tokenVector->push_back(WideString());
                        tokenCount = tokenCount + 1;
                    }
                }
                else
                {
                    tokenVector->push_back(mid(str, nextStartIdx, newLen));
                    tokenCount = tokenCount + 1;
                }
                tokenVector->push_back(WideString(*d1));
                tokenCount = tokenCount + 1;
                nextStartIdx = i + 1;
            }
        }
    }
    if (nextStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}