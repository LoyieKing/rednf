bool arad_make_item(unsigned long itemIndex, int count, Inven_Item& out)
{
    if (itemIndex == 0 || (int)itemIndex == -1 || count == 0)
        return false;
    CItem* item = G_CDataManager()->find_item((int)itemIndex);
    if (item == 0)
        return false;
    out.reset();
    item->make_item(out);
    out.m_addInfo  = (int)itemIndex;   // +0x02（ORIG mov %edx,0x2(%eax)）
    out.m_addInfo2 = count;            // +0x07（ORIG mov %edx,0x7(%eax)）
    return true;
}