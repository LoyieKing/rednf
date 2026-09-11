// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
unsigned int WideString::getHash() const
{
    // [L4e 2026-09-11 · 负结论：38/39，残余＝回边 jmp 后 1 条 `nop`] ORIG 0x08ad9cd2
    // `cmpl $0,-0x14(%ebp)` 直接紧随 0x08ad9cd0 `jmp`（无填充）；根因同 CharString::getHash
    // （4.4.x 循环尾 nop / GCC 4.1.2 无，8 旗标无效）⇒ 源码层不可修。
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
}