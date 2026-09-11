// ORIG 0x085522c4 (T, _ZNK)：map find，未找到返回 0
CMailBox::CMail* CMailBox::GetMail(unsigned int mailIndex) const
{
    MailMap::const_iterator it = m_mailMap.find(mailIndex);
    if (it == m_mailMap.end())
        return 0;
    return it->second;
}