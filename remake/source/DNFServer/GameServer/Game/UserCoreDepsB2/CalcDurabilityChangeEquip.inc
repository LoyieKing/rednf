// ---------------------------------------------------------------------------
// 2. AvatarRechargeServer::CalcDurabilityChangeEquip —— ORIG T 0x081905b4
// （报告 AvatarRechargeServer/CalcDurabilityChangeEquip.md）
// 状态 5 且组队中、疲劳副本、未清图时，槽位 0..9 执行 CalcDurabilityDetail。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] AE-：P0 去 party 具名局部（ORIG 三次独立 call GetParty）
// + P225 dead 变体 `while (!call()) break;` 复现 `xor $1; test %al,%al`，71→77
// （ORIG 78）。残 1 条 = N104 EH 尾块相对序：ORIG 把共享 dtor 段置于 EH 岛**之后**
// （正常路径 `jmp +0xc7`），4.4 置于 EH 岛**之前**（正常路径 fallthrough，少 1 jmp）。
// 守卫 nop 岛 4 组、setne/sete 物化、寻址/槽位逐条同 ORIG（ORIG 0x081905b4）。语义等价。
// [L4b 2026-09-11 复核] 权威口径（compare_tu_game_orig.clean）DIFF 74/73、ndiff=7：
// 唯一残差即上述共享 dtor 段位移 3 行（ORIG 正常路径就地析构后 `jmp +0xc7`，4.4 把
// 正常路径指向 EH 岛后的 epilogue 前共享段）——4.4 cleanup-sharing，源码不可择 ⇒ N104。
void AvatarRechargeServer::CalcDurabilityChangeEquip(CUser* user, int equipSlot)
{
    if (user->get_state() != 5)
        return;
    if (user->GetParty() == 0)
        return;
    if (!user->GetParty()->IsUseFatigueDungeon())
        return;
    if (user->GetParty()->CheckMapClear())
        return;

    std::vector<int> rates;            // ORIG 栈 vector（空构造即传入）
    if (-1 < equipSlot && equipSlot < 10)
    {
        while (!CalcDurabilityDetail(user, equipSlot, rates))
            break;
    }
}