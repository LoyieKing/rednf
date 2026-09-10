int STExpertJobScript::GetAlchemistExtractionPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it =
        m_alchemistExtractInfo.find(level);
    if (it == m_alchemistExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
