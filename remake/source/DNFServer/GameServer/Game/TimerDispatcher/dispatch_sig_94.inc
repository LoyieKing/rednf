int TimerStartMatching::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, 0x79);
    if (room == NULL) {
        return 0;
    }
    // ORIG 0x08638xxx：room 单槽 -0xc，matcher 每次从 room+0x6e4 直读（三次
    // `mov -0xc,%eax; mov 0x6e4(%eax),%eax`，无具名槽）；虚表槽 +0x38，
    // 实参为 p2（0x10(%ebp)）非 p3。
    if (((TimerStartMatchingMatcherSlot*)room)->m_matcher == 0)
        return 0;
    ((TimerStartMatchingMatcherSlot*)room)->m_matcher->OnMatchRoom(room, p2);
    return 1;
