CItem* CDataManager::find_item(int idx) const
{
    if (idx == 0 || idx == 1 || idx == 2)
    {
        return 0;
    }
    return m_itemList->find_item(idx);
