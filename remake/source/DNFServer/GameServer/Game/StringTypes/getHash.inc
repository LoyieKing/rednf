unsigned int CharString::getHash() const
{
    // [b28d-solo-StringTypes] verdict=AE：残差仅 N112 循环出口对齐 nop（§6.1 豁免）。
    // [b28d-34] AE（compare_tu 口径，ndiff 2→0）：残差仅 4.4 循环出口 nop 填充
    // （4.1 无；-falign-loops=1/-fno-align-jumps 实测无效）⇒ §6.1 对齐 nop 豁免；
    // 归一后逐条全等。ORIG 0x08ad6c1c。
    // ORIG：isEmpty 结果直接 test %al（bool 返回值不物化），空串走 mov $0,%eax
    // 常量岛提前 return；value=0 初始化在检查之后落槽。
    if (isEmpty())
        return 0;
    unsigned int value = 0;
    const char* str = buffer_;
    while (true)
    {
        char c = *str;
        str = str + 1;
        if (c == '\0')
            break;
        value = ((value << 6) + (value << 16)) - value + static_cast<int>(c);
    }
    if (value == 0)
        value = 1;
    return value;
