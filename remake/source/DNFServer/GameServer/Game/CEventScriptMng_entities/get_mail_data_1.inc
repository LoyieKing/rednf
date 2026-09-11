char* CAttendance::get_mail_data()
{
    // ORIG 0x810e7da：return &m_mail。
    return reinterpret_cast<char*>(&m_script1c->m_mail);
}