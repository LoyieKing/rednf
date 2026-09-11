// ----------------------------------------------------------------------------
// ORIG T 0x08686df4 (_ZN15cUserHistoryLog12AbnormalUserE27ENUM_ABNORMAL_USER_LOG_TYPEtii)：
//   eax = *(this)（+0x00 m_user，无空检查）；以
//   m_user->LogHistory("AbnormalUser,%u,%hu,%d,%d", type, value, a3, a4) 转发。
// 格式串 ORIG @0x8cf1037 已核对。
// ----------------------------------------------------------------------------
void cUserHistoryLog::AbnormalUser(ENUM_ABNORMAL_USER_LOG_TYPE type,
                                   unsigned short value, int a3, int a4)
{
    m_user->LogHistory("AbnormalUser,%u,%hu,%d,%d",
                       (unsigned int)type, (unsigned int)value, a3, a4);
}