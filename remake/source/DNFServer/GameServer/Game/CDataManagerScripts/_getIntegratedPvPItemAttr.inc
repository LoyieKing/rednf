int _getIntegratedPvPItemAttr(const Inven_Item& item)
{
    CItem* it = G_CDataManager()->find_item(item.m_addInfo);
    if (it == 0)
    {
        return 0;
    }
    if (it->GetItemType() == 0xa)
    {
        return (int)item.GetReSealCount() << 5;
    }
    if (it->GetItemType() > 0xb && it->GetItemType() <= 0x15)
    {
        return (int)item.GetReSealCount() << 5;
    }
    return item.GetItemAttr();
