std::string* dungeonEventHanaseStruct::getNewDialog(ENUM_CHARACTERJOB job)
{
    if (m_field24 != 0 && static_cast<int>(job) >= 0 &&
        static_cast<int>(job) <= 10 && m_dialogs[job] != 0)
    {
        return m_dialogs[job];
    }
    return &m_str20;
