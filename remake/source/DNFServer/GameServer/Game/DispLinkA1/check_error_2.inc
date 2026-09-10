unsigned int Dispatcher_CreateDisjointStore::check_error(CUser* user, MSG_BASE& msg,
                                                ParamBase& param)
{
    (void)msg;
    (void)param;
    if ((user->get_state() <= 2) ? true : (user->getCurCharacR() == 0))
        return -1;
    if (user->GetCurCharacExpertJobType() != 3)
        return -2;
    if (user->get_state() != 3)
        return 0x13;
    if (user->GetCurCharacExpertJob() != 0)
        return 0x13;
    if (user->GetParty() != 0)
        return 0x13;
    return 0;
