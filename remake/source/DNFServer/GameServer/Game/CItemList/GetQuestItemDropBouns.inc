// ORIG W 0x0869476e _ZNK9CItemList21GetQuestItemDropBounsERKm：
// m_lottoCount(+0x5c, std::map<unsigned long,int>).find(key)；
// 命中返 second（mov 0x4 偏移），未命中返 0。
int CItemList::GetQuestItemDropBouns(const unsigned long& key) const
{
    std::map<unsigned long, int>::const_iterator it = m_lottoCount.find(key);
    if (it != m_lottoCount.end())
    {
        return it->second;
    }
    return 0;
}