// ----------------------------------------------------------------------------
// ORIG 0x08646912 CUser::OnItemUnlockWaitTimeout(int expandIdx=2, int arg2)
// uint，实义返回 !CItemLock::OnItemUnlockWaitTimeout
// [b28d-22 N96+N18a] ORIG 0x08646941 尾 `xor $0x1,%eax; test %al,%al; leave; ret`
// 的 test 是 4.1 bool 归一 dead test（无分支消费者）。4.4 实测（/tmp/p22_unlock.cpp
// g1..g7，446h46 路由）：uint 形只出 `xor; leave; ret`（缺 test），bool 形反产
// test/sete/movzbl（P168a③）⇒ 无源形，封顶；语义等价（test 不改返回值）。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08646912：P168a③ 封顶——ORIG 尾 `xor $1,%eax;
// test %al,%al; leave; ret` 的 test 是 4.1 bool 归一 dead test（无分支消费者）；4.4 uint/bool
// 两形均只出 `xor; leave; ret`（g1..g7 探针）⇒ 语义等价，仅缺 1 条 dead test。
unsigned int CUser::OnItemUnlockWaitTimeout(int expandIdx, int unused)
{
    // ORIG 0x08646912：`movl $0x2,0x4(%esp)` 立即数 ⇒ 硬编常量 2（非 expandIdx 变量）；
    // 首 call 结果 %eax 跨步直落第二 call 首参槽（无具名 lock 槽，帧 0x18）
    // ORIG 0x08646912：xor $0x1;test %al——gcc4.4 对 `^1` 返回前冗余 test。
    // 本工具链 `^1`/`!` 均不产冗余 test（ndiff 1 死 test 残差，N 类）。
    return (unsigned int)(((item_lock::CItemLock*)
        GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2))
        ->OnItemUnlockWaitTimeout(this) ^ 1);
}