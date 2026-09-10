bool DB_ArrangeUserLoginInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_ARRANGE_USER_LOGIN_INFO* sig =
        stream->GetOutBuffer<SIG_ARRANGE_USER_LOGIN_INFO>();
    if (sig->m_flag == 1)
        GlobalData::s_psimpleSSO->expireLoginTime(sig->m_accId);
    return 1;
