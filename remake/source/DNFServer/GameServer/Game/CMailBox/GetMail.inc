CMailBox::CMail* CMailBox::GetMail(unsigned int mailIndex) const
{
    MailMap::const_iterator it = m_mailMap.find(mailIndex);
    if (it == m_mailMap.end())
        return 0;
    return it->second;
