char const* WongWork::CWebEvent::_GetEventString(int idx)
{
    // ORIG T 0x086c1ba8 (size 0x2b)：idx≤0xa 允许（含第 11 槽，照抄 asm jle
    // 边界）；stride 0x12。ret 栈变量形态按 ORIG 字节序（movl $0,-0x4(%ebp)
    // 初始化；if (idx>0xa) return ret → cmpl+jle 跳计算块）。
    char const* ret = 0;
    if (idx > 0xa)
    {
        return ret;
    }
    return &m_strEventName[0][0] + idx * 0x12;
