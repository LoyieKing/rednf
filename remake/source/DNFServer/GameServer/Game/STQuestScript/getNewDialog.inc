// ORIG T 0x8a615fe（0x44）：m_field24!=0 且 0<=job<=10 且 m_dialogs[job]!=0 →
// 返回 m_dialogs[job]；否则返回 &m_str20（+0x20）。
std::string* dungeonEventHanaseStruct::getNewDialog(ENUM_CHARACTERJOB job)
{
    if (m_field24 != 0 && static_cast<int>(job) >= 0 &&
        static_cast<int>(job) <= 10 && m_dialogs[job] != 0)
    {
        return m_dialogs[job];
    }
    return &m_str20;
}