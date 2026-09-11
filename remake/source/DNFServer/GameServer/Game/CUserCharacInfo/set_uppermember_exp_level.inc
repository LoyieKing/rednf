void CUserCharacInfo::set_uppermember_exp_level(unsigned char level)
{
    if (m_selected)
    {
        if (level != 0 && CUR->m_uppermemberExpLevel > level)
        {
            LogManager::logFormat(1, "user.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x2d2, "set member exp level error , name(%s) , level(%d), new level(%d)",
                                  (char*)CUR->m_name, (unsigned int)CUR->m_uppermemberExpLevel,
                                  (unsigned int)level);
        }
        else
        {
            CUR->m_uppermemberExpLevel = level;
        }
    }
}