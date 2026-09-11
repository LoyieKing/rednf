int CMap::insert_item(int idx, int level, int prob, int cond)
{
    _mapItem item;
    item.m_objectIdx = idx;
    item.m_field04 = level;
    item.m_field08 = prob;
    item.m_field0c = cond;
    m_itemList.push_back(item);
    return 1;
}