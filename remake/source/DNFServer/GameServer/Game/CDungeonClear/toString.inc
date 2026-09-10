int CDungeonClear::toString(char* buf, unsigned int bufSize) const
{
    if (m_map.empty())
    {
        return 0;
    }

    // b28d-60：P50 直构——原「先声明后赋值」触发两个 iterator 默认 ctor
    // C1Ev（帧 0x44 vs ORIG 0x34、+20 insn）。ORIG 顺序：empty → offset/written
    // 清零 → begin 直构 it → 首段 snprintf → ++it → end 直构 → while。
    int offset = 0;
    int written = 0;
    std::map<short, char>::const_iterator it = m_map.begin();
    // ORIG 首段 snprintf 结果直存 -0x10(offset)，无 written 中转（written 仅
    // 在循环内使用，函数头仍需 `int written = 0;` 的 -0xc 清零）。
    offset = OS_API::snprintf(buf, bufSize, "%d|%d",
                              (int)it->first, (int)it->second);
    ++it;
    std::map<short, char>::const_iterator end = m_map.end();
    while (it != end)
    {
        written = OS_API::snprintf(buf + offset, bufSize - offset,
                                   ",%d|%d", (int)it->first,
                                   (int)it->second);
        if (written < 0)
        {
            return -1;
        }
        offset += written;
        ++it;
    }
    return offset;
