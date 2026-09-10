bool CCharacInformNotice::loadData(CUser* user, char* data)
{
    _reset();
    memcpy(m_flags, data, sizeof(m_flags));
    sendInformNoticeFlag(user);
    return true;
