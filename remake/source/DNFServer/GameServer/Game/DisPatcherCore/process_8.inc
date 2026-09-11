// ORIG 0x081c3dd8 T _ZN22DisPatcher_GetUserInfo7processEP5CUserR8MSG_BASER9ParamBase
// err=check_error；>0 返 -1；<0 LineFunc(0xf6d,pretty,0,acc_id)（第三参恒 0）；
// 否则 G_GameWorld()->get_user_info(user, (u16)msg+0xd, (s8)msg+0xf)，返回 err。
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
}
