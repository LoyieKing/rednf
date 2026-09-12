// ORIG T 0x0808c74b _Z13PI_GetType_SCcs：type==1 查表、type==2 if 链、缺省 0。
int PI_GetType_SC(char type, short value)
{
    // [b28d-solo-OnlyOrigBatch14] ORIG 0x0808c75e `movsbl -0x4(%ebp),%eax;
    // cmp $0x1,%eax; je; cmp $0x2,%eax; je` = 二路 switch/if 链的整字提升形；
    // type==1 为跳表 switch（sub $0x13/cmp $0x40/ja/jmp *table），type==2 为
    // 稀疏 switch 二分决策树（P46：叶块排布 = case 书写顺序）。两侧 case
    // 顺序按 ORIG 叶块地址序重排（0x13..0x53 / 0x28..0x85）。
    // 判词·AE 归零（100=100）：内层 switch 不写 default、改 switch 后
    // `break;` + 函数尾单一 `return 0`，使所有失败路径共享 ORIG 0x0808c8ae
    // 的 `mov $0x0,%eax` 尾块（P17/P46）。
    switch (type)
    {
    case 1:
        switch (value)
        {
        case 0x1f: return 9;
        case 0x1d: return 11;
        case 0x2f: return 12;
        case 0x2e: return 18;
        case 0x48: return 22;
        case 0x49: return 23;
        case 0x4a: return 24;
        case 0x20: return 25;
        case 0x13: return 26;
        case 0x43: return 27;
        case 0x53: return 44;
        }
        break;
    case 2:
        switch (value)
        {
        case 0x28: return 4;
        case 0x8e: return 5;
        case 0x8f: return 6;
        case 0x1c: return 7;
        case 0x76: return 13;
        case 0x25: return 16;
        case 0x27: return 17;
        case 0x73: return 19;
        case 0x22: return 20;
        case 0x23: return 21;
        case 0x85: return 39;
        }
        break;
    }
    return 0;
}

// ORIG T 0x0808bc7e _Z6Rand_rPj：LCG rand_r（NULL 时用全局种子）。
// [b28d-solo-OnlyOrigBatch14 复核 lane24 判词·N 封顶] 唯一残差 = NULL 分支
// `staticSeed = timeGetTime() + staticSeed`：ORIG 0x0808bc8f `mov %eax,%edx;
// mov staticSeed,%eax; lea (%edx,%eax,1),%eax`（RHS-first + lea），4.4 恒
// `mov staticSeed,%edx; add %edx,%eax`（LHS-first + add）。P211 负例（static
// 存储期变量复合赋值装载序不可翻，b28c C28L24 三救均败）+ add/lea 跨版本；
// 其余 49 条与 ORIG 全等。
// [P1T1 本波复核] 残差复核 = 仅 NULL 分支 3 条（ORIG 0x0808bc8f `mov %eax,%edx;
// mov staticSeed,%eax; lea (%edx,%eax,1),%eax` vs 我方 `mov staticSeed,%edx;
// add %edx,%eax`）；维持 P211 static 装载序 + add/lea 跨版本封顶。
unsigned int Rand_r(unsigned int* seed)
{
    // ORIG _ZZ6Rand_rPjE10staticSeed 0x0936e480：`.data` 初值 raw = b1 7f 39 05
    // = 87654321（R20scanEC raw 复核）。NULL 分支 staticSeed += timeGetTime() 参与种子链，
    // 初值 0 会改变随机序列。
    static unsigned int staticSeed = 87654321u;
    if (seed == 0)
    {
        // [b28c-lane24] ORIG 0x0808bc82：`call timeGetTime; mov %eax,%edx;
        // mov staticSeed,%eax; lea (%edx,%eax,1),%eax` = RHS 值先（P211 符号性
        // 异型 ⇒ RHS-first）；LHS/RHS 同为 unsigned 时 4.4 恒 LHS-first
        //（`mov staticSeed,%edx; add %edx,%eax`）⇒ 交换书写序探测（P211 无效：
        // staticSeed int 化反使取址多一条 eax 中转，已回退）。
        staticSeed = timeGetTime() + staticSeed;
        return Rand_r(&staticSeed);
    }
    unsigned int next = *seed;
    // 拆句 *=/+= 复现 ORIG addl $K,-off 内存加与 shll mem 内存移
    // [b28c-lane24] result 由 unsigned 改 int：ORIG xor 段 `mov %eax,%edx;
    // and $0x3ff,%edx; mov -0xc(%ebp),%eax; xor %edx,%eax; mov %eax,-0xc(%ebp)`
    // = RHS 先物化（P211 异型）；同型时 4.4 恒折 `and %eax; xor %eax,mem`。
    next *= 0x41c64e6d;
    next += 0x3039;
    int result = (next >> 16) & 0x7ff;
    next *= 0x41c64e6d;
    next += 0x3039;
    result <<= 10;
    result = result ^ ((next >> 16) & 0x3ff);
    next *= 0x41c64e6d;
    next += 0x3039;
    result <<= 10;
    result = result ^ ((next >> 16) & 0x3ff);
    *seed = next;
    return result;
}