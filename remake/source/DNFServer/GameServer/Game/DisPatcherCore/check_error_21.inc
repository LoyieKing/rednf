unsigned int DisPatcher_DieCharacInDungeon::check_error(CUser* user, MSG_BASE& msg)

{
    // ORIG 0x081c2bc0：`xor $0x1,%eax; test; je` = if(!check) return -1; return 0。
    if (!Global::CGlobalFunc::Get_State_Check((ch_state)user->get_state()))
        return -1;
    return 0;
}
