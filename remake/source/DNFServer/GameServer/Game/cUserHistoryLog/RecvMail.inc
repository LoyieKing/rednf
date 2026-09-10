void cUserHistoryLog::RecvMail(unsigned int mailIdx, const char* senderName,
                               const char* accName)
{
    m_user->LogHistory("MailR,%d,\"%s\",\"%s\"", mailIdx, senderName, accName);
