void CEventScriptMng::process_account_first_login(
    int key, CUser* user, std::bitset<32u> flags)
{
    // P133 独立守卫；confirmFlag 无栈槽（P0：值经 %edx 直传 operator[] 实参）。
    if (user == 0)
        return;
    if (!is_eventing(key))
        return;

    if (!flags[get_convert_to_user_confirmflag(key)])
    {
        CEventEntity* entity = get_event_entity(key);
        if (entity != 0)
            static_cast<CAccountFirstLogin*>(entity)->do_action(
                key, user, flags);
    }
