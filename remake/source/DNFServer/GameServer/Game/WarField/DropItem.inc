int WarField::DropItem(map_item item)
{
    int old = m_mapInfo.m_dropItemCnt++;   // ORIG lea 0x1(%eax) 单链自增
    item.m_itemIndex = old;
    m_mapInfo.Add_Item(item);
    return old;
