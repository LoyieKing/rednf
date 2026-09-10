int TimerSetTradePunishUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // ORIG 0x08638c48：两段独立早退；比较对象是 p2（0x10(%ebp)）非 p3；
    // get_unique_id 返回 unsigned short ⇒ movzwl %ax 后直比，无 &0xffff。
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
    user->SetTradePunishType(4);
    return 1;
