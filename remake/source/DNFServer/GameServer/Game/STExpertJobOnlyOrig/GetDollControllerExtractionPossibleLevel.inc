int STExpertJobScript::GetDollControllerExtractionPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it = m_dollExtractInfo.find(level);
    if (it == m_dollExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
