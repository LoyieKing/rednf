void CUserCharacInfo::set_uppermember_exp_level(unsigned char level)
{
    if (m_selected)
    {
        // [DNF-FIX 2026-10-08 c25] ORIG 0x08645ba2: movzbl 0xe32 ; cmp ; jbe  (unsigned byte compare)
        if (level != 0 && (unsigned char)m_selected->m_uppermemberExpLevel > level)
        {
            LogManager::logFormat(1, "user.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x2d2, "set member exp level error , name(%s) , level(%d), new level(%d)",
                                  (char*)m_selected->m_name, (unsigned int)(unsigned char)m_selected->m_uppermemberExpLevel,
                                  (unsigned int)level);
        }
        else
        {
            m_selected->m_uppermemberExpLevel = level;
        }
    }
}