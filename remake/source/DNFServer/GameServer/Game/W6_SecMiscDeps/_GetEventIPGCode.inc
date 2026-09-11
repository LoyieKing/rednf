char const* WongWork::CWebEvent::_GetEventIPGCode(int idx)
{
    // ORIG T 0x086c1bd4 (size 0x4d)：idx>9 或槽首字节为 0 时返回 NULL；
    // stride 0xb。ORIG 形：p 预置槽（idx>9 分支 mov -0x4,%eax 读槽），
    // body 两处独立完整地址计算（movzbl 直用内联地址；return 处重算），
    // 条件 jle→body（fall-through = idx>9）。
    char const* p = 0;
    if (idx > 9)
    {
        return p;
    }
    if (m_strEventIPGCode[idx * 0xb] == '\0')
    {
        return 0;
    }
    return m_strEventIPGCode + idx * 0xb;
}