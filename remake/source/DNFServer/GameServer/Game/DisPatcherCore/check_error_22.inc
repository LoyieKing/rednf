// ORIG 0x081ca7d6 T _ZN18DisPatcher_UseCoin11check_errorEP5CUserR8MSG_BASER9ParamBase
// （报告 DisPatcher_UseCoin/check_error.md 逐块映射）：user->get_state()==5 →
// Dungeon_check_error；==8 → WarRoom_check_error；其余 → 0x13。
unsigned int DisPatcher_UseCoin::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    if (user->get_state() == 5)
    {
        return Dungeon_check_error(user, msg, param);
    }
    if (user->get_state() == 8)
    {
        return WarRoom_check_error(user, msg, param);
    }
    return 0x13;
}
