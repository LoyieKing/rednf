unsigned int CDataManager::GetMoneyLimitPerLevel(int level, const char* serverGroup) const
{
    if (level <= 0 || level > 200)
    {
        LogManager::logFormat(1, "data_manager.cpp",
                              "CDataManager::GetMoneyLimitPerLevel", 0x2705,
                              "Out of range level (%d)", level);
        level = std::max(1, std::min(200, level));
    }
    if (serverGroup && strcmp(serverGroup, "0") == 0)
    {
        return 0x7fffffff;
    }
    return m_moneyLimit[level];
