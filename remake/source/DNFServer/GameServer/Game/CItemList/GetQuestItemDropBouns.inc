int CItemList::GetQuestItemDropBouns(const unsigned long& key) const
{
    std::map<unsigned long, int>::const_iterator it = m_lottoCount.find(key);
    if (it != m_lottoCount.end())
    {
        return it->second;
    }
    return 0;
