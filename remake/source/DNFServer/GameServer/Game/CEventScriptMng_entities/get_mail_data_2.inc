char* CAccountFirstLogin::get_mail_data()
{
    // ORIG 0x810decc：return &m_mail。
    return reinterpret_cast<char*>(
        &static_cast<AccountFirstLoginScript*>(m_script1c)->m_mail);
