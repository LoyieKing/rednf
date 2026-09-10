int DisPatcher_GetUserInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;   // ORIG 不写 param
    const int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        return -1;
    }
    if (errNo < 0)
    {
        return LineFunc(0xf6d,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    MSG_USERINFO_REQ& req = *reinterpret_cast<MSG_USERINFO_REQ*>(&msg);
    G_GameWorld()->get_user_info(user,
                                 static_cast<int>(req.m_targetId),
                                 static_cast<signed char>(req.m_slot));
    return errNo;
