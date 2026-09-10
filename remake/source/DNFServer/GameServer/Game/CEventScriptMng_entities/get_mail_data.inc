char* CLevelUpReward::get_mail_data()
{
    // ORIG 0x810e2e4：return &m_mail。
    return reinterpret_cast<char*>(
        &static_cast<LevelUpRewardScript*>(m_script1c)->m_mail);
