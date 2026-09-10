unsigned int Dispatcher_RequestDisjointItem::check_error(CUser* user, MSG_BASE& msg,
                                                ParamBase& param)
{
    if ((user->get_state() <= 2) ? true : (user->getCurCharacR() == 0))
        return -1;

    if (ARAD::Singleton<ServiceRestrictManager>::Get()     // 0x081625e6（结果直传）
            ->isRestricted(user, RestrictCategory::Enum(1), 0x13))
        return 0xd1;

    ParamUidView_A2* pUid = reinterpret_cast<ParamUidView_A2*>(&msg);  // ORIG mov 0x10(%ebp) 即 msg
    CUser* target = G_GameWorld()->find_from_world(pUid->m_uid);
    if (target == 0)
        return 3;
    return 0;
