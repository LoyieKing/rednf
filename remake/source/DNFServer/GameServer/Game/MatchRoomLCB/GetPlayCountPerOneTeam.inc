// ---------------------------------------------------------------------------
// 3. CFairMatch::GetPlayCountPerOneTeam —— ORIG 0x08562834 恒返 1。
//    [b28d-solo-MatchRoomLCB] N 封顶（§6.1 nop 豁免 + 切片伪差，源码侧无可动）：
//    ORIG nm size 0x15 = 函数体 5 条（+0x00..+0x09，与本实现逐字全等：
//    push/mov/mov$1/pop/ret，我方 .o size 0x0a）+ 10×nop 对齐（+0x0a..+0x13）
//    + 孤立 ret（+0x14，位于本符号范围内、下一函数 CFairMatch::C1Ev 0x856284a 之前）。
//    4.4 对 `return 1;` 不可能在 ret 后再发 nop×10+ret ⇒ 切片伪差不可消；
//    p167 调用点差集 CLEAR（0/0）；Ghidra C 亦为 `return 1;`。
// ---------------------------------------------------------------------------
int CFairMatch::GetPlayCountPerOneTeam()
