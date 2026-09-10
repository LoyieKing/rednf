unsigned int CUser::OnItemUnlockWaitTimeout(int expandIdx, int unused)
{
    // ORIG 0x08646912：`movl $0x2,0x4(%esp)` 立即数 ⇒ 硬编常量 2（非 expandIdx 变量）；
    // 首 call 结果 %eax 跨步直落第二 call 首参槽（无具名 lock 槽，帧 0x18）
    // ORIG 0x08646912：xor $0x1;test %al——gcc4.4 对 `^1` 返回前冗余 test。
    // 本工具链 `^1`/`!` 均不产冗余 test（ndiff 1 死 test 残差，N 类）。
    return (unsigned int)(((item_lock::CItemLock*)
        GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2))
        ->OnItemUnlockWaitTimeout(this) ^ 1);
