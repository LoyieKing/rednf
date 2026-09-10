void CCharacInformNotice::getData(char* data) const
{
    memcpy(data, m_flags, sizeof(m_flags));
