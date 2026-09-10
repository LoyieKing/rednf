int CUser::get_character_job(unsigned int characNo) const
{
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator mark = m_characList.end();
    while (it != mark)
    {
        if (static_cast<unsigned int>(it->m_characNo) == characNo)
            return static_cast<int>(it->m_field26);  // +0x26 job（movsbl）
        ++it;
    }
    return -1;
