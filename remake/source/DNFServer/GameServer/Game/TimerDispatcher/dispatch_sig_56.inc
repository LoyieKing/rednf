int TimerForbidMove::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 0xb);   // ORIG 第二实参常量 0xb
    if (user == 0)
        return 0;
    // 与 TimerForbidChat 同构（ORIG 0x086360a8）：register name→%ebx、
    // 各点重新调用、!EnableMove 前置、Failed fall-through。
    register const char* name = user->get_charac_name_by_charac_no(p3);
    cMyTrace(__PRETTY_FUNCTION__, 0xa64, 0)("TimerForbidMove %s", name);
    name = user->get_charac_name_by_charac_no(p3);
    if (!G_GameWorld()->EnableMove(name))
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa66, 0)("Failed to enable to move for: %s", name);
    }
    else
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa68, 0)("Enabled to move for: %s", name);
    }
    return 1;
