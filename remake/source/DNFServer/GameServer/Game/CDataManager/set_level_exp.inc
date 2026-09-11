bool CDataManager::set_level_exp()
{
    for (int i = 0; i <= 199; ++i)
    {
        m_expTable[i] = g_levelExpTable[i];
    }
    return 1;
}