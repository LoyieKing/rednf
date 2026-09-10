void cUserHistoryLog::AbnormalUser(ENUM_ABNORMAL_USER_LOG_TYPE type,
                                   unsigned short value, int a3, int a4)
{
    m_user->LogHistory("AbnormalUser,%u,%hu,%d,%d",
                       (unsigned int)type, (unsigned int)value, a3, a4);
