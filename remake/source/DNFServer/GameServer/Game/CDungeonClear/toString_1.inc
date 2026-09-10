int CDungeonClear::toString(std::string& out, int nUnknown) const
{
    if (m_map.empty())
        return 0;

    // b28d-60：按 ORIG 0x08484558 逐条重写——原实现（clear + resize(+64) 固定
    // 缓冲 + first 标志）与 ORIG 完全不同。ORIG：resize(nUnknown) 一次，
    // curSize(-0x24)=nUnknown、written(-0x20)=0，首段 snprintf("%d|%d") 用
    // (c_str, curSize)，循环内 snprintf(c_str+written, curSize-written, ",%d|%d")，
    // 返回负则 resize(curSize+nUnknown)、curSize+=nUnknown 后重试同元素。
    int curSize = nUnknown;
    out.resize((unsigned int)nUnknown);
    int written = 0;
    int w = 0;
    std::map<short, char>::const_iterator it = m_map.begin();
    written = OS_API::snprintf(const_cast<char*>(out.c_str()),
                               (unsigned int)curSize, "%d|%d",
                               (int)it->first, (int)it->second);
    ++it;
    std::map<short, char>::const_iterator end = m_map.end();
    while (it != end)
    {
        // ORIG 0x084846a7 `jmp 0x0848462a`（回循环体头，非条件块）⇒ 内层重试循环
        // `while(true){ ...; if (w>=0) break; resize; curSize+=nUnknown; }`；
        // 用 `continue` 会跳条件块（jmp 目标下标差 42），与 ORIG CFG 不同。
        while (true)
        {
            w = OS_API::snprintf(const_cast<char*>(out.c_str()) + written,
                                 (unsigned int)(curSize - written), ",%d|%d",
                                 (int)it->first, (int)it->second);
            if (w >= 0)
                break;
            out.resize((unsigned int)(nUnknown + curSize));
            curSize += nUnknown;
        }
        written += w;
        ++it;
    }
    return written;
