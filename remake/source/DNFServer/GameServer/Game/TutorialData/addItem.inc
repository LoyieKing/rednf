void TutorialData::addItem(int itemId, int count)
{
    ItemData* item = new ItemData;
    item->m_itemId = itemId;
    item->m_count = count;
    m_items.push_back(item);
