bool TimerVillageAttackedPlayTimeout::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x27, p2)) {
        return 0;
    }
    // ORIG 0x086391a3：`mov -0xc,%eax; movzwl 0xc4c(%eax),%eax; movzwl %ax,%eax;
    // cmp 0x14(%ebp),%eax` 单级折叠（view 嵌套成员链式，无 field 具名槽）。
    if (static_cast<unsigned short>(
            reinterpret_cast<CPartyBattleFieldView*>(party)
                ->m_battleField.m_villageMonsterId) != p3) {
        return 0;
    }
    party->OnFinishVillageMonsterFighting((CUser*)0, true);
    return 1;
}