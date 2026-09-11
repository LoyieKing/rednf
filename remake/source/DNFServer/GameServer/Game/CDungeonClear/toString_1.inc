// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
int CDungeonClear::toString(std::string& out, int nUnknown) const
{
    if (m_map.empty())
        return 0;

    // b28d-60：按 ORIG 0x08484558 逐条重写——原实现（clear + resize(+64) 固定
    // 缓冲 + first 标志）与 ORIG 完全不同。ORIG：resize(nUnknown) 一次，
    // curSize(-0x24)=nUnknown、written(-0x20)=0，首段 snprintf("%d|%d") 用
    // (c_str, curSize)，循环内 snprintf(c_str+written, curSize-written, ",%d|%d")，
    // 返回负则 resize(curSize+nUnknown)、curSize+=nUnknown 后重试同元素。
    // [L6S3] 档③同型化（P211/P274）：ORIG 0848468c `mov 0x10(%ebp),%eax;
    // add -0x24(%ebp),%eax`（形参 int 先载 + 内存 add）与 08484653
    // `mov %edx,%edi; sub %eax,%edi`（单段直减）⇒ curSize 侧为 unsigned 同型
    // 运算；我方旧形写 `(unsigned int)(nUnknown + curSize)`/`(unsigned int)
    // (curSize - written)` 恒多 eax/ebx 中转而 ORIG 无。curSize 声明改
    // unsigned int，去掉全部显式窄化 cast。
    unsigned int curSize = (unsigned int)nUnknown;
    out.resize((unsigned int)nUnknown);
    int written = 0;
    int w = 0;
    std::map<short, char>::const_iterator it = m_map.begin();
    written = OS_API::snprintf(const_cast<char*>(out.c_str()),
                               curSize, "%d|%d",
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
                                 curSize - written, ",%d|%d",
                                 (int)it->first, (int)it->second);
            if (w >= 0)
                break;
            out.resize(nUnknown + curSize);
            curSize += nUnknown;
        }
        written += w;
        ++it;
    }
    return written;
}