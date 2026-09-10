int Dispatcher_InformNotice::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 3)
    {
        return 1;
    }
    const MSG_INFORM_NOTICE& req = reinterpret_cast<const MSG_INFORM_NOTICE&>(msg);
    // ORIG 0x081e3008（报告 Dispatcher_InformNotice/check_error.md 全文已读）：
    // 尾块 jbe→return 0（真体后置），fall-through return -1。
    if ((unsigned int)req.m_noticeFlag > 0x7f)
    {
        return -1;
    }
    return 0;
