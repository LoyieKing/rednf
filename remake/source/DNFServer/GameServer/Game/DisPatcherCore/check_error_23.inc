// ORIG 0x081c41e8 T _ZN21DisPatcher_GiveUpGame11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_GiveUpGame/check_error.md 逐块映射）：state ∈
// {5,10,12,13} → 0；其余 → 0x13（ORIG 每次比较前调 get_state，勿缓存）。
int DisPatcher_GiveUpGame::check_error(CUser* user, MSG_BASE& msg)

{
    if (user->get_state() != 5 && user->get_state() != 10
        && user->get_state() != 0xc && user->get_state() != 0xd)
    {
        return 0x13;
    }
    return 0;
}
