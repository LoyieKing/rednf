// ORIG W 0x0808c174：unsigned char* 重载（ORIG 中与 char* 版同为独立展开）。
int DoSuperFastHash(unsigned char* data, unsigned int len, unsigned int& hash)
{
    // [b28d-L1] 同 char* 版修法（ORIG W 0x0808c174）：len 作循环变量、单一 rem
    // 累加、分步就地 `rem op=`、逐字节 movzbl 组装。
    // [SweepPIHash] 续修同 char* 版（139/138）：`(data[3]<<8)+data[2]`、t 前置声明、
    // 尾部 `rem += rem>>6; hash = rem;` 分两步；残余为尾部 if 链内存直比（N）。
    hash = 0;
    unsigned int rem = len;
    if (len == 0 || data == 0)
        return 0;

    // [SweepPIHash] 同 char* 版：t 声明前置于 tmp（ORIG t@-0x8 / tmp@-0x4）。
    unsigned int t;
    unsigned int tmp = len & 3;
    for (len >>= 2; len != 0; --len)
    {
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        // ORIG `(data[3]<<8) + data[2]`（lea），非 `|`。
        t = (((unsigned int)data[3] << 8) + (unsigned int)data[2]) << 11 ^ rem;
        rem = (rem << 16) ^ t;
        data += 4;
        rem += rem >> 11;
    }
    // [SemA13-slice13] 残差①：ORIG 0x0808c21f 决策链为 `mov -0x4(%ebp),%eax;
    //   cmp $0x2,%eax; je; cmp $0x3,%eax; je; cmp $0x1,%eax; jne`（寄存器比较 +
    //   fall-through 落 case 1 块）= switch 形态；if/else 链在 4.4 恒折 mem 直比
    //   （`cmpl $0x2,-0x4(%ebp)`）⇒ 改 switch，case 书写序照 ORIG 决策序 2/3/1。
    // 残差②（真语义差异）：ORIG case 3 与 case 1 的字节装载均为 `movzbl (%eax),%eax;
    //   movzbl %al,%eax`（0x080c2b7/0x080c238 实证）⇒ 原码为无符号取字节；我方
    //   `(unsigned int)(int)(signed char)data[n]` 产 `movsbl`（data[n]>=0x80 时
    //   高位置 1、与 ORIG 结果不同）⇒ 删 (int)(signed char) 双重 cast。
    // [SemA13-slice13] case 书写序 = ORIG 叶块物理序 1/2/3（P324：叶块物理序=书写序；
    //   ORIG 0x0808c235(case1) → 0x0808c254(case2) → 0x0808c288(case3)）。
    switch (tmp)
    {
    case 1:
        rem += (unsigned int)data[0];
        rem ^= rem << 10;
        rem += rem >> 1;
        break;
    case 2:
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        rem ^= rem << 11;
        rem += rem >> 17;
        break;
    case 3:
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        rem ^= rem << 16;
        rem ^= (unsigned int)data[2] << 18;
        rem += rem >> 11;
        break;
    }
    rem ^= rem << 3;
    rem += rem >> 5;
    rem ^= rem << 4;
    rem += rem >> 17;
    rem ^= rem << 25;
    // 同 char* 版：ORIG 分两步就地更新 rem 再读入 hash。
    rem += rem >> 6;
    hash = rem;
    return 1;
}