bool Arad_Script::Load(const char* dir, const char* file)
{
    if (!Create())
        return 0;
    char* pStart = 0;
    char tag[0x400] = { 0 };
    std::sprintf(tag, "[%s]", file);
    unsigned int i = 0;
    unsigned int tokenLen = 0;
    int pos = 0;
    int tagLen = (int)std::strlen(tag);
    int c = 0;
    int rowIdx = 0;
    int colLen = 0;
    int unused = 0;
    (void)unused;

    // 节标记滑动匹配（ORIG +0xad..+0x101：空白/可打印外字符忽略）
    while (true)
    {
        c = (int)(signed char)*dir++;
        if (c == -1)
            return 0;
        if (c <= 0x20)
        {
        }
        else if (c > 0x7e)
        {
        }
        else if ((int)(signed char)tag[pos] == c)
        {
            ++pos;
            if (pos == tagLen)
                break;
        }
        else
            pos = 0;
    }

    // 列名行（ORIG +0x103..+0x34d）：'/' 分段键各写 colIndex_[seg]=usedCols_，
    // 末段写完后 usedCols_ 自增一次并按需 ExpandCol。
    char colBuf[0x200] = { 0 };
    colLen = 0;
    for (;;)
    {
        c = (int)(signed char)*dir++;
        if (c == -1)
            return 0;
        if (c == 0x20 || c == 9 || c == 10 || c == 13)
        {
            if (colLen > 0)
            {
                colBuf[colLen] = 0;
                colLen = 0;
                pStart = colBuf;
                tokenLen = (unsigned int)std::strlen(colBuf);
                for (i = 0; i < tokenLen; ++i)
                {
                    if (colBuf[i] == '/')
                    {
                        colBuf[i] = 0;
                        {
                            std::allocator<char> alloc;
                            std::string key(pStart, alloc);
                            colIndex_[key] = usedCols_;
                        }
                        pStart = colBuf + i + 1;
                    }
                }
                {
                    std::allocator<char> alloc;
                    std::string key(pStart, alloc);
                    colIndex_[key] = usedCols_;
                    usedCols_ = usedCols_ + 1;
                }
                if (usedCols_ + 1 >= cols_)
                {
                    if (!ExpandCol())
                        return 0;
                }
            }
        }
        else
        {
            colBuf[colLen] = (char)c;
            ++colLen;
        }
        if (c == 10)
            break;
    }

    // 数据块行（ORIG +0x34f..+0x544）：rowIdx=-1 起；首 token 建行名，
    // 之后逐格 strcpy；行满后跳至行尾/EOF；行结束但 rowIdx!=usedCols_ → 失败。
    while (true)
    {
        rowIdx = -1;
        colLen = 0;
        while (true)
        {
            c = (int)(signed char)*dir++;
            if (c == 0x20 || c == 9 || c == 10 || c == 13 || c == -1)
            {
                if (colLen > 0)
                {
                    colBuf[colLen] = 0;
                    colLen = 0;
                    if (rowIdx == -1)
                    {
                        {
                            std::allocator<char> alloc;
                            std::string key(colBuf, alloc);
                            rowIndex_[key] = usedRows_;
                        }
                    }
                    else
                    {
                        std::strcpy(cells_ + ((rowIdx + (usedRows_ * cols_)) << 7), colBuf);
                    }
                    ++rowIdx;
                    if (rowIdx == usedCols_)
                    {
                        while (c != -1 && c != 10)
                            c = (int)(signed char)*dir++;
                    }
                }
                else
                {
                    c = -1;
                }
                if (c == 10 || c == -1)
                    break;
                continue;
            }
            colBuf[colLen] = (char)c;
            ++colLen;
        }
        if (usedCols_ != rowIdx)
            return 0;
        ++usedRows_;
        if (usedRows_ + 1 >= rows_)
        {
            if (!ExpandRow())
                return 0;
        }
        if (c == -1)
            break;
    }
    return 1;
