// [b28d-D43] N4 scaled 写 vs lea 折叠：ORIG `movb $0,(%edx,%eax,1)` vs 4.4 `lea (%edx,%eax,1); movb $0,(%eax)`（ORIG 0x08899dfa）。
void itemGloballyUniqueIdentifier_t::reset()
{
    // ORIG 0x85139e8：循环变量为 char（movsbl 读/addb 自增/cmpb $9 判界）。
    // [b28c-33 判词 NEAR-N4] ORIG 写侧为 `movsbl i,%eax; mov this,%edx; movb $0,
    // (%edx,%eax,1)`（索引先+scaled 直存）；4.4 对 `(char*)this[i]` 恒折
    // `mov this,%edx; movsbl i,%eax; lea (%edx,%eax,1); movb $0,(%eax)`（+1 条）。
    // 探针 l33d f1..f5：仅「成员数组」形出 scaled，本类无 +0 起 char[10] 成员
    // （需 union 视图改头，收益 1 条，未做）。15/16。
    // [P1 round5 二轮·新钥匙实测] `*(reinterpret_cast<char*>(this) + i) = 0`（指针加法形）
    //   与 `(...)[i]`（下标形）**逐字节同形**（`mov this,%edx; movsbl i,%eax; lea
    //   (%edx,%eax,1),%eax; movb $0,(%eax)`，16 条不变）⇒ 4.4 的 lea 折叠是 peephole 固定形，
    //   非源码可造。ORIG 0x085139f4 的 `movb $0,(%edx,%eax,1)`（无 lea）确需成员数组视图
    //   （改头，收益 1 条）⇒ 维持封顶（同 l33d f1..f5 判词）。
    // [G16 2026-09-12 · P288 数组引用形 **落地改善**] ORIG 0x08899dfa 为
    // `movb $0,(%edx,%eax,1)`（scaled 直存、无 lea）；旧下标形/指针加法形
    // （l33d f1..f5）恒产 lea 折叠（16 条）。
    // 按 P288（`char (&t)[N]` / `(*p)[i]` 数组引用视图 ⇒ scaled 直存）改后
    // **16→15 条 = ORIG 15（gap -1→0）**，仅剩 2 条装载序差
    //（ORIG `movsbl i,%eax` 先于 `mov this,%edx`；4.4 恒 base 先）⇒ 属 P288 附
    // 求值序残差，已试并回滚：具名引用局部 17（占槽）、`register char i` 17（push %ebx）、
    // `i[arr]` 交换下标 15（同形）、双重 cast 15（同形）⇒ 保留本形（改善登记）。
    for (char i = 0; i <= 9; ++i)
        (*reinterpret_cast<char (*)[10]>(this))[i] = 0;
}