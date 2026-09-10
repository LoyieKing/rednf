int DisPatcher_FinishLoad::check_state(CUser* user, PacketBuf& /*packet*/)

{
    if (!Global::CGlobalFunc::Get_State_Check(
            static_cast<ch_state>(user->get_state())))
    {
        return 0x7fffffff;
    }
    return 0;
}
