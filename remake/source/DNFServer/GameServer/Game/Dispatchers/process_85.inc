int Dispatcher_Mercenary_Info::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ddb46 (53 insn): !user -> -1; user->get_state() <= ch_state1 -> -1; else 0
    if (user == 0)
        return -1;
    if (user->get_state() <= static_cast<int>(ch_state1))
        return -1;
    return 0;
}