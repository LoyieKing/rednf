// ORIG T 0x863ad96（cancel_quick_party.md 全文转写；成员函数，需 TimerDispatcher.h 补声明）
// [b28d-30] 复核 DIFF：帧差 0x10（0x20 vs 0x30）+ 槽差（N82/P210 族），ORIG 0x0863ad96。
// [b28d-solo-TimerDispatcher2] P194/P222 + P52：ORIG `test %al,%al; je <mov $1,%ebx>`
// = **正极性包裹**（旧码 `!call` 多 xor 物化）；ORIG 无 mgr 具名槽（`call
// GetQuickPartySystemManager; mov %eax,(%esp)` 直传）⇒ 内联 receiver 链；party 槽
// 由 -0x10 回 -0xc（ORIG）。
bool Timer_QuickPartyWaitingPartyMatching::cancel_quick_party(
    QuickParty::CQuickParty* quickParty)
{
    CParty* party = quickParty->get_party_object();
    if (party == 0)
    {
        return false;
    }
    if (G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(
            party, true, static_cast<CUser*>(0)))
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x17f);
        packet.finalize(true);
        party->send_to_party(packet);
        return false;
    }
    return true;
}