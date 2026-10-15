// ORIG 0x8541e4e：item 空间字节(+0x01)==8 → UpdateAvatarItem(0x1c2)；
// ==5 → UpdateCreatereItem(0x1c1)。（注意 ORIG 判定次序：==8 先，调用的是 Avatar。）
void CItemLock::UpdateItem(CUser* user, Inven_Item* item)
{
    if (item->m_itemType == 8)
        UpdateAvatarItem(user, item);
    else if (item->m_itemType == 5)
        UpdateCreatereItem(user, item);
}