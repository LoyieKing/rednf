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
