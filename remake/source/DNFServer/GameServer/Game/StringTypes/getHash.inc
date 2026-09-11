// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
unsigned int CharString::getHash() const
{
    // [L4e 2026-09-11 · 负结论：38/39，残余＝回边 jmp 后 1 条 `nop`] ORIG 0x08ad6c7c
    // `cmpl $0,-0x14(%ebp)` 直接紧随 0x08ad6c7a `jmp`（无填充）。根因＝后端版本行为：
    // 同形最小复现 /tmp/exp/t1.cpp(-m32) 在 cc1plus 4.4.4/4.4.6/4.4.7 均生成该 nop，
    // c5root GCC 4.1.2 同源输出无 nop 且逐条全等；8 个候选旗标（-fno-exceptions/
    // -falign-loops=1/-fno-reorder-blocks/…）无一消除 ⇒ 源码层不可修；整 TU 换 4.1.2
    // 亦不可行（HashMap.h 等 4.4-only 语法编译失败）。
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
}