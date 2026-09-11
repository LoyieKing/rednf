int CharString::tokenize(const CharString& str, std::vector<CharString>* tokenVector,
                         const char* dropDelimiters, const char* returnDelimiters,
                         bool isKeepEmptyToken)
{
    // [b28d-solo-StringTypes] 收敛（ORIG 0x08ad8c6c）：①ORIG 的 `*d0` 判定为
    // `test *d0; je <远处 d1 分支>` + drop 分支直落；4.4 对原 if/else 顺序产
    // 反向布局（jne 跳 else + d1 分支直落），交换两臂并取反条件即复现块序。
    // ②按 P212 全头部声明（i/c/d0/d1/drop-newLen，声明序=ORIG 槽深序）复现
    // ORIG 槽位 i=-0x20/c=-0x19/d0=-0x18/d1=-0x14/newLen=-0x10。
    const char* strBuf = str.c_str();
    int strLen = str.length();
    int nextStartIdx = 0;
    int tokenCount = 0;
    int i;
    char c;
    const char* d0;
    const char* d1;
    int newLen;
    for (i = 0; i < strLen; ++i)
    {
        c = strBuf[i];
        d0 = dropDelimiters;
        while (*d0 != '\0' && *d0 != c)
            ++d0;
        if (*d0 != '\0')
        {
            newLen = i - nextStartIdx;
            if (newLen == 0)
            {
                if (isKeepEmptyToken)
                {
                    tokenVector->push_back(CharString());
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
            while (*d1 != '\0' && *d1 != c)
                ++d1;
            if (*d1 != '\0')
            {
                int newLen = i - nextStartIdx;
                if (newLen == 0)
                {
                    if (isKeepEmptyToken)
                    {
                        tokenVector->push_back(CharString());
                        tokenCount = tokenCount + 1;
                    }
                }
                else
                {
                    tokenVector->push_back(mid(str, nextStartIdx, newLen));
                    tokenCount = tokenCount + 1;
                }
                tokenVector->push_back(CharString(*d1));
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