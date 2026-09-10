unsigned int WideString::getHash() const
{
    // [b28d-solo-StringTypes] verdict=AE：残差仅 N112 循环出口对齐 nop（§6.1 豁免）。
    // [b28d-34] AE（同 CharString::getHash，nop 豁免）。ORIG 0x08ad9c74。
    // ORIG：isEmpty 结果直接 test %al（bool 返回值不物化），空串走 mov $0,%eax
    // 常量岛提前 return；value=0 初始化在检查之后落槽。
    if (isEmpty())
        return 0;
    unsigned int value = 0;
    const wchar_t* str = buffer_;
    while (true)
    {
        int c = *str;
        str = str + 1;
        if (c == 0)
            break;
        value = ((value << 6) + (value << 16)) - value + c;
    }
    if (value == 0)
        value = 1;
    return value;
