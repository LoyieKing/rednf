// ORIG Timer_DungeonInoutCloseTime::dispatch_sig @ 0x863a1d0 (0x25f)
// [b28d-30] 复核 DIFF：帧差 0x10（0xa4 vs 0xb4，N82/P210 族），ORIG 0x0863a1d0。
// [b28d-solo-TimerDispatcher2] P167 差集 MISSING G_CDataManager×1 ⇒ ORIG find/end
// **各自重发** `G_CDataManager()`+`lea 0x130`（无 dataMgr 局部缓存，P157/P183）；
// P0：ORIG 0863a213 `mov 0x14(%ebp),%eax; mov %eax,-0x58(%ebp)` 全程读 -0x58
// ⇒ 原码有 p3 的具名副本（dungeonIdx），旧码形参直读。
    // [TimerDispatcher-3 2026-09-10 判据①] 9 条 jcc 目标逐条对应（jns 位移 1 由单条指令差
    // 引起）；唯一多重集差 = ORIG `mov -0x58,%eax;test %eax,%eax` vs 我方
    // `cmpl $0x0,-0x58`（/tmp/td_probe_cmp.cpp 六形 + /tmp/td_probe2.cpp 八形恒 cmpl）
    // ⇒ 旧判词 N18b/N22 封顶。
    // [A12-2026-09-10 · C05 ② **命中**（旧 N18b 判词作废）] 该具名局部
    // `unsigned int dungeonIdx` → **int**（S1 一次性旋钮）后，4.4 即由 mem 直比
    // `cmpl $0x0,-0x58` 翻为 ORIG 的载寄存器两段形 `mov -0x58(%ebp),%eax; test`：
    // 本件 **DIFF→NEAR**，TU 四数 diff 13→12 / near 4→5（identical 60 / ae 700 不变）。
    // 边界留痕：此处比较是**零比较（test 形）**而非 `cmp $imm`，与 Main 汇总的
    // 「常量 vs 局部不适用」边界不同 ⇒ 供边界更新参考（cmp $imm 子型仍归 N18b）。
bool Timer_DungeonInoutCloseTime::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
