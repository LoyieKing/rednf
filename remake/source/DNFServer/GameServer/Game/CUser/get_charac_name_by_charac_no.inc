const char* CUser::get_charac_name_by_charac_no(int characNo) const
{
    // ORIG 0x0863bece：循环下标 int，size 有符号比较（setg）。
    for (int i = 0; i < static_cast<int>(m_characList.size()); ++i)
    {
        if (m_characList[i].m_characNo == characNo)
            return m_characList[i].m_name;
    }
    return 0;
