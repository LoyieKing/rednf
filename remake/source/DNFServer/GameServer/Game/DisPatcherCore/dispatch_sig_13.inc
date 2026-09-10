int DisPatcher_IncreStatus::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    // [b28e-L5] P250：ORIG 载荷为 pack(1) 结构体局部（槽基 -0x2f、val@+0x0d，
    // `lea -0x2f(%ebp),%eax; add $0xd,%eax` 两步取址；0x081ede5a/0x081ede7c 等多处
    // `movzwl -0x22(%ebp),%eax` 即 m_val 重读）。
    IncreStatusSigView sig;
    if (!packet.get_short(sig.m_val))
    {
        return LineFunc(0x70fc, __PRETTY_FUNCTION__, 0, 0);
    }
    CItem* pItem = GlobalData::s_pSecuProtectionField->GetItemPtr(
        user, (char)0, (short)sig.m_val);
    if (pItem == 0)
    {
        return LineFunc(0x7101, __PRETTY_FUNCTION__, 0, 0);
    }
    int secu = GlobalData::s_pSecuProtectionField->CheckItem(
        user, static_cast<SECURITY_PROTCTION>(0x2a), pItem);
    if (secu != 0)
    {
        PacketGuard guard;
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x20, (unsigned char)secu, guard);
        return 0;
    }
    // [SigB] P59/P222 正/负极性包裹：ORIG 0x081edf8c `cmp $0x98,%ax` +
    // 0x081edf90 `jle 081edfee <+0x1ba>` ⇒ 取反跳 **else**，且 else 块
    // （increase_status）位于 +0x1ba（then=UseQuestItem 走 fall-through）。
    // 原 `sig.m_val < 0x99` 正形使 4.4 反产 `jg`＋then 前置（偏差 2 条 jcc/块序）。
    // `0x98 < x` 与 `x < 0x99` 对 signed short 等价（无符号性/宽度变化）。
    if (0x98 < sig.m_val)
    {
        user->UseQuestItem((int)sig.m_val);
        // [b28e-L5] P227 全表达式临时链（ORIG 0x081ede9x 在 C1 call 前右起求 varargs
        // 并 `movswl %ax,%ebx` 跨 ctor 保活 ⇒ 单语句链式）。
        cMyTrace(__PRETTY_FUNCTION__, 0x710f, 5)("__ USE QUEST ITEM __ %d",
                                                 (int)sig.m_val);
        // [DPC-SigC 2026-09-11] ORIG 0x081edfe7 `mov $0x0,%ebx` + 0x081edfec
        // `jmp 081ee007`（`mov %ebx,%eax` 前）：trace 段自带**独立**返回 0 物化。
        // 我方原 fall-through 复用尾部唯一 `mov $0x0,%ebx`（0x12b57），少 1 条
        // （LCS del ORIG[108:109]，our 122 vs ORIG 123）。补显式 return 复现该点；
        // 语义等价（两路径本就返回 0）。预期 ours 122→123，差异块 1→0。
        return 0;
    }
    else
    {
        user->increase_status((short)sig.m_val);
    }
    return 0;
}
