int CDataManager::get_level_exp(int level) const
{
    if (level < 2 || level > 200)
    {
        return 0;
    }
    return m_expTable[level - 2];
}