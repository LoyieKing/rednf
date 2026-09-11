// ORIG 0x081cb062 T _ZN23DisPatcher_WalkoutParty11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_WalkoutParty/check_error.md 逐块映射）：user 空 →
// LineFunc(0x2053)；state==3 → 0；否则 -1。
int DisPatcher_WalkoutParty::check_error(CUser* user, MSG_BASE& msg)

{
    if (user == 0)
    {
        return LineFunc(0x2053,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x081cb062：`cmp $3; setne; test; je` = if(!=3) return -1; return 0。
    if (user->get_state() != 3)
        return 0xffffffff;
    return 0;
}
