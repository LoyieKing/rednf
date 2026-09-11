// [SweepW8LT b28d] DIFF：gap=-21（N82 槽差 + N7/N18a 寄存器分配）；ORIG 0843097e
// [M8DBA r5] gap −22 → +3：ORIG 1033 / our 1055 → 1036。
//   手法：badges/dailyBadges 共 12 处访问改「整型索引先行指针形」
//   `((DB_UpdatePvPResultSlotView*)(buf + (r * 3 + p * 0x38)))->m_badges[0].x`
//   —— ORIG 直证（0x0843155b / 0x0843173f 区，如
//   `mov -0x1c(%ebp),%ecx; mov -0x20(%ebp),%edx; mov -0x34(%ebp),%ebx;
//    mov %edx,%eax; add %eax,%eax; add %edx,%eax; mov %ecx,%edx; shl $0x3,%edx;
//    lea 0x0(,%edx,8),%ecx; sub %edx,%ecx; add %ecx,%eax; lea (%ebx,%eax,1),%eax;
//    add $0x1d,%eax; movzbl (%eax),%eax`）：ORIG 是「3r+56p 整型和先行 + 加基址」，
//   我方旧形是 `(buf + p*0x38) + r*3` 分段（每处多 `mov %ecx,%ebx; sub; mov %ebx,%eax`
//   2 条，extra 计数实证 19 处）。12 处 × 2 = 24 条已消。
bool DB_UpdatePvPResult::dispatch(int userIdx, int slot, Stream* stream)
