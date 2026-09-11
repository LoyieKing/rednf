int stNPCCommonData_t::getIllustIndex(int npcIdx)
{
    for (unsigned int i = 0; i < m_illustrationVariableNpcList.size(); ++i)
    {
        if (m_illustrationVariableNpcList.at(i) == npcIdx)
            return i;
    }
    return -1;
}