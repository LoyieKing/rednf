// ORIG T 0x08120242：无当前角色/槽空物品均 false；否则按上一函数判定。
bool IsTradeLimitAttachTypeItemByUser(CUser& user, int space, int slot)
{
    if (user.getCurCharacR() == 0)
        return false;
    Inven_Item item = user.getCurCharacInvenR()->GetInvenSlot(space, slot);
    if (item.m_addInfo == 0)
        return false;
    return IsTradeLimitAttachTypeItem(item);
}