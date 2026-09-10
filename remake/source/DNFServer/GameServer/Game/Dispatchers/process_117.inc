int Dispatcher_ChangeLetterStat::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stChangeLetterStatParam& out = static_cast<stChangeLetterStatParam&>(param);
    int mailIdx;
    int letterStat;
    int result;

    out.m_result = check_error(user, msg);
    if (out.m_result > 0)
        return 0;
    if (out.m_result < 0)
    {
        LineFunc(0x2744,
                 __PRETTY_FUNCTION__,
                 out.m_result, user->get_acc_id());
        return 0;
    }
    mailIdx = reinterpret_cast<MSG_CHANGE_LETTER_STAT&>(msg).m_mailIdx;   // +0xd
    letterStat = (int)reinterpret_cast<MSG_CHANGE_LETTER_STAT&>(msg).m_letterStat;  // +0x11 short
    result = WongWork::CMailBoxHelper::ReqChangeLetterStat(user, mailIdx, letterStat);
    out.m_result = result;
    if (out.m_result == 0)
        return 0;
    return 0;
