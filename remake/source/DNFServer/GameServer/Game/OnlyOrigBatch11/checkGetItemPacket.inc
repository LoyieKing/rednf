// ORIG T 0x0889209d —— 逐段反汇编定语义（b28c-l23 修正旧"奇偶相异"判词）：
// ①`-0x8 = b + c`；`-0x2 = a & 1`；`-0x1 = sum & 1`（三条独立槽 store）；
// ②@+0x24 `cmpb $0,-0x2; je L1` + @+0x2a `cmpb $0,-0x1; jne Lret0`
//   ⇒ 第一段 = `(oddA == 0) || (oddSum == 0)`（与零比较 ⇒ 直分支 cmpb 形）；
// ③L1 @+0x30 `movzbl -0x2; xor $1; test; je Lret1` + @+0x3b 同族测 -0x1
//   ⇒ 第二段 = `(!oddA) || (!oddSum)`（bool 取反 ⇒ P168a X 族物化形）；
// ④两段的真出口共享 Lret1（mov $1），假出口共享 Lret0（mov $0）。
// 合并即"至少一个为偶 ⇒ 1；两者皆奇 ⇒ 0"（偶偶也返 1）。
bool checkGetItemPacket(int a, int b, int c)
{
    const int sum = b + c;
    // [b28d-solo-OnlyOrigBatch11] 语义订正 + 形复现：
    // 真值表（ORIG 逐路径推演）：11→0、10→1、01→1、00→0，即 `oddA ^ oddSum`。
    // 旧实现 `!oddA || !oddSum` 在 00 情形返 1 ⇒ 真语义 bug（P196 族），已按 ORIG 改正。
    // 形：两槽必须是 **bool** 局部（P292 同源）——uchar 局部下 4.4 把取反折成
    // `cmpb $0;je` 直分支，bool 局部才发 ORIG 的 `movzbl; xor $1; test %al,%al; je`。
    // [LaneA 改形 · 依据 = ORIG 块图反推] ORIG 0x0889209d 的块图**不含任何 goto**：
    //   第一段 @+0x24 `cmpb $0,-0x2; je L1` + @+0x2a `cmpb $0,-0x1; jne Lret0`
    //     ⇒ 原码 `if (oddA) { if (oddSum) return 0; }`（A 假落 L1、B 真直落 ret0）；
    //   第二段 @+0x30/@+0x3b 两条 `xor $1; test; je Lret1`（**共享同一目标** +0x4d）
    //     ⇒ 原码 `if (!oddA) { if (!oddSum) return 0; } return 1;`
    //     （ret0 块 @+0x46 直落 ret1 块 @+0x4d）。
    // 旧 goto 形（`if (oddA&&oddSum) goto Lfail; … goto Lsucc;`）语义等价，但 4.4 会
    // 为 goto 目标多布 3 处 jmp/nop 中继岛（ORIG 0 岛）⇒ 换成上述**自然 if/else 形**。
    // 本形未经本泳道编译（纪律：禁止编译），由父泳道 fresh 复验裁定；若仍劣化则回退
    // 至旧 goto 形（探针 /tmp/b28d_s83_probe3.cpp h2 族，21 形读数见该探针）。
    const bool oddA = (a & 1) != 0;
    const bool oddSum = (sum & 1) != 0;
    if (oddA)
    {
        if (oddSum)
            return 0;
    }
    if (!oddA)
    {
        if (!oddSum)
            return 0;
    }
    return 1;
}