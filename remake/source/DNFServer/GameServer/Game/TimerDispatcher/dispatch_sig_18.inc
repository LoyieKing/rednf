bool TimerPremiumServiceOver::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 5);   // ORIG 第二实参为常量 5
    // ORIG 0x0863485a：三段独立早退（jne 岛 + setne 物化 + xor$1），非 || 链。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->IsPremiumUser())
        return 0;
    WongWork::CHandlePremium::handlePremiumTimeout(user);   // ORIG 0x863485a
    return 1;
}