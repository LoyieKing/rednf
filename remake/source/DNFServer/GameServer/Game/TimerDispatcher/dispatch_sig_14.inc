int TimerCardSelect::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x10, p2))
    {
        return 0;
    }
    // ORIG 0x08634412..+0x84：|| 合并条件 —— gcc4.4 对 || 生成短路跳转 +
    // 汇合点 mov$1/jmp/mov$0 物化（探针 x4 p1 实证）；独立两 if 各自 sete，不合 ORIG。
    if (party->get_dungeon_clear_state() == 0 ||
        party->get_dungeon_clear_state() == 5)
    {
        return 1;
    }
    if (!party->IsCompleteCardSelectOfMembers(eClearRewardCardType_t_0))
    {
        party->CompleteCardSelect();
        party->SendEachCardInfo();
    }
    party->set_dungeon_clear_state(5);
    party->dungeon_clear();
    return 1;
