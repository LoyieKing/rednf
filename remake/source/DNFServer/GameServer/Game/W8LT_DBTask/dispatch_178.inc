// ORIG 0x0842acf4 sz 0x3b：GetOutBuffer<SIG_ARRANGE_USER_LOGIN_INFO>；
// sig+0x04(byte)==1 → s_psimpleSSO->expireLoginTime(*(uint*)sig+0)；返 1。
bool DB_ArrangeUserLoginInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_ARRANGE_USER_LOGIN_INFO* sig =
        stream->GetOutBuffer<SIG_ARRANGE_USER_LOGIN_INFO>();
    if (sig->m_flag == 1)
        GlobalData::s_psimpleSSO->expireLoginTime(sig->m_accId);
    return 1;
}