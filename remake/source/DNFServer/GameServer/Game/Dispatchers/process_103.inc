int Dispatcher_MailBox_Extract::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stMailboxExtractParam& p = static_cast<stMailboxExtractParam&>(param);
    p.m_4 = check_error(user, msg);
    if (p.m_4 > 0)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        LineFunc(0x245d,
                 __PRETTY_FUNCTION__,
                 p.m_4, user->get_acc_id());
    }
    else
    {
        MSG_MAILBOX_EXTRACT_ITEM& mref = reinterpret_cast<MSG_MAILBOX_EXTRACT_ITEM&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
        WongWork::CMailBoxHelper::GetPackege(user, mref.m_mailIdx);
    }
    return 0;
