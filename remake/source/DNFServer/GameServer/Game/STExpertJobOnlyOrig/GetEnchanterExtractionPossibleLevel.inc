int STExpertJobScript::GetEnchanterExtractionPossibleLevel(int level)
{
    std::map<int, unsigned short>::iterator it =
        m_enchanterExtractInfo.find(level);
    if (it == m_enchanterExtractInfo.end())
        return -1;
    return static_cast<int>(it->second);
}