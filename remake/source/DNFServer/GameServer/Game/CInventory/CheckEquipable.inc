bool CheckEquipable(const Inven_Item& a, const Inven_Item& b)
{
    if (a.m_addInfo != 0)
    {
        if (a.m_itemType != Inven_Item::ITEM_TYPE_EQUIP &&
            a.m_itemType != Inven_Item::ITEM_TYPE_AVATAR)
        {
            return false;
        }
    }
    if (b.m_addInfo != 0)
    {
        if (b.m_itemType != Inven_Item::ITEM_TYPE_EQUIP &&
            b.m_itemType != Inven_Item::ITEM_TYPE_AVATAR)
        {
            return false;
        }
    }
    if (a.m_addInfo == 0 && b.m_addInfo == 0)
    {
        return false;
    }
    return true;
}