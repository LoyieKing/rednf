void CEventScriptMng::process_add_fatigue_count(CUser* user, int count)
{
    if (user == 0)
        return;
    if (!is_eventing(0x56))
        return;
    user->set_server_fatigue_count((short)count);
