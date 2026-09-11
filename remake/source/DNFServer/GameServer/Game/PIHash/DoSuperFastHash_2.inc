// ---- DoSuperFastHash ORIG @ 0x0808ca7a ----
// [b28d-L1] 修法：ORIG 用**形参 len 本身**作循环变量（`shrl $0x2,0xc(%ebp)` 就地
// 改形参槽）、累加只有 rem 一个局部（无独立 h/i），循环体与尾分支均为分步
// `rem op= …` 就地写（`add %eax,-0xc(%ebp)` / `xor %eax,-0xc(%ebp)` 实证），
// 且字节按 `movzbl (%eax); movzbl %al,%eax` 逐字节组装（P241 窄化形，非 movzwl）。
// 原单表达式形使 4.4 多 3 个局部槽（-0x10/-0xc/-0x8/-0x4）、循环尾 setne 形、
// 字节读 movzwl，ndiff 121（r48）。按 ORIG 分步形重写后逐条对齐。
// [SweepPIHash] 续修（139/138，gap 1）：
//   ① 中段改 `(data[3]<<8) + data[2]`（ORIG `lea (%edx,%eax,1)`）；旧写 `|` 产 `or`；
//   ② t 声明前置于 tmp（ORIG t@-0x8 / tmp@-0x4；4.4 函数头标量按声明序先深）；
//   ③ 尾改 `rem += rem>>6; hash = rem;` 两步（ORIG `add %eax,-0xc(%ebp)` 就地写；
//      单表达式 `hash = rem + (rem>>6)` 4.4 恒产 edx 中转）。
//   残余 N：尾部 `if(tmp==2/3/1)` 链 4.4 恒 `cmpl $K,mem; jne`×3（6 条）vs ORIG
//   `mov tmp,%eax; cmp $2,%eax; je; cmp $3,%eax; je; cmp $1,%eax; jne`（4.1 装载一次，
//   7 条）；switch 形 4.4 产 mov+cmp/je×3+jmp（8 条，多 1 条 default jmp）⇒ 两形均不闭合。
//   [P1T5 probe sw1] 已实测真 switch：ours 140（ORIG 139）、444/446 后端同 140 ⇒ 非后端差异，
//   是 4.4 decision-tree 布局不做末 case fall-through（末位恒 `je`+`jmp default`，ORIG 用 `jne`），
//   源码不可翻，回滚。
int DoSuperFastHash(char* data, unsigned int len, unsigned int& hash)
{
    hash = 0;
    unsigned int rem = len;
    if (len == 0 || data == 0)
        return 0;

    // [SweepPIHash] ORIG t 槽 -0x8 深于 tmp 槽 -0x4 ⇒ t 须声明在 tmp 之前
    // （4.4 函数头标量按声明序「先声明深」，与 ORIG 一致）。
    unsigned int t;
    unsigned int tmp = len & 3;
    for (len >>= 2; len != 0; --len)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        // ORIG 用 `(data[3]<<8) + data[2]`（lea 合并），非 `|`（or 形）。
        t = (((unsigned int)(unsigned char)data[3] << 8) +
             (unsigned int)(unsigned char)data[2]) << 11 ^ rem;
        rem = (rem << 16) ^ t;
        data += 4;
        rem += rem >> 11;
    }
    // [P1T5 probe sw1 · 回滚] switch 形实测 ours=140（ORIG 139，更差 1 条）：
    // 4.4 产 `mov tmp,%eax; cmp $2/je; cmp $3/je; cmp $1/je; jmp Lend`（8 条），
    // 比 ORIG 的 7 条（末位改 jne 的 fall-through 布局）多一条 default jmp；
    // 444/446 后端同形（各 140）⇒ 4.4 系布局不做末 case fall-through，源码无解。
    if (tmp == 2)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        rem ^= rem << 11;
        rem += rem >> 17;
    }
    else if (tmp == 3)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        rem ^= rem << 16;
        rem ^= (unsigned int)(int)(signed char)data[2] << 18;
        rem += rem >> 11;
    }
    else if (tmp == 1)
    {
        // ORIG movsx（signed char）——与 tmp==3 的 data[2] 一致。
        rem += (unsigned int)(int)(signed char)data[0];
        rem ^= rem << 10;
        rem += rem >> 1;
    }
    rem ^= rem << 3;
    rem += rem >> 5;
    rem ^= rem << 4;
    rem += rem >> 17;
    rem ^= rem << 25;
    // ORIG 分两步（`add %eax,-0xc(%ebp)` 就地更新 rem 再读入 hash），
    // 单表达式 `hash = rem + (rem>>6)` 4.4 恒产 edx 中转形。
    rem += rem >> 6;
    hash = rem;
    return 1;
}