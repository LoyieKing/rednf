unsigned long Event_Upgrade_Jar::GetIndexLuckyStrengtheningDeed()
{
    if (m_materialIndices.size() >= 2)
        return m_materialIndices[1];
    return 0;
