// ORIG 0x080f4fd0。m_materialIndices.size()>=2 时返回 [1]，否则 0。
unsigned long Event_Upgrade_Jar::GetIndexLuckyStrengtheningDeed()
{
    if (m_materialIndices.size() >= 2)
        return m_materialIndices[1];
    return 0;
}