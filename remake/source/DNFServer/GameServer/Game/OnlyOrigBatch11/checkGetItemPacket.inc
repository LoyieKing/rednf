bool checkGetItemPacket(int a, int b, int c)
{
    const int sum = b + c;
    // [b28d-solo-OnlyOrigBatch11] 语义订正 + 形复现：
    // 真值表（ORIG 逐路径推演）：11→0、10→1、01→1、00→0，即 `oddA ^ oddSum`。
    // 旧实现 `!oddA || !oddSum` 在 00 情形返 1 ⇒ 真语义 bug（P196 族），已按 ORIG 改正。
    // 形：两槽必须是 **bool** 局部（P292 同源）——uchar 局部下 4.4 把取反折成
    // `cmpb $0;je` 直分支，bool 局部才发 ORIG 的 `movzbl; xor $1; test %al,%al; je`。
    // 第二段用 `if (!oddA) { if (!oddSum) goto Lfail; } goto Lsucc;`（探针
    // /tmp/b28d_s83_probe3.cpp h2，另穷举 probe4/5/6 共 15 形）：xor 物化 +
    // je Lsucc ×2 + 失败 fall-through 全中；fndiff clean 归一后 ndiff=2，残差仅
    // 2 条 jmp 中继（4.4 为 goto 目标多布，nop 已被 clean 剥离）⇒ N 封顶
    // （跨版本 CFG 块布局，P63/P223 族；21 形探针均不可消）。
    const bool oddA = (a & 1) != 0;
    const bool oddSum = (sum & 1) != 0;
    if (oddA && oddSum) goto Lfail;
    if (!oddA)
    {
        if (!oddSum) goto Lfail;
    }
    goto Lsucc;
Lfail:
    return 0;
Lsucc:
    return 1;
