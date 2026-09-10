bool IsTradeLimitAttachTypeItemByUser(CUser& user, int space, int slot)
{
    if (user.getCurCharacR() == 0)
        return false;
    Inven_Item item = user.getCurCharacInvenR()->GetInvenSlot(space, slot);
    if (item.m_addInfo == 0)
        return false;
    return IsTradeLimitAttachTypeItem(item);
