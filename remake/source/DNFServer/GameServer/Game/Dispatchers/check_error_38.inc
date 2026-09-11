int Dispatcher_InformNotice_2nd::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 3)
    {
        return 1;
    }
    const MSG_INFORM_NOTICE_2ND& req = reinterpret_cast<const MSG_INFORM_NOTICE_2ND&>(msg);
    if ((unsigned int)req.m_noticeFlag > 0x7f)
    {
        return -1;
    }
    return 0;
}