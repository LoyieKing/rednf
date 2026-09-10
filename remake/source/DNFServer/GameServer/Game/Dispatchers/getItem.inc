int Dispatcher_UseRenameCard::getItem(const CUser& user, int invenType, short slot,
                                      Inven_Item& out) const
{
    if (user.CheckItemLock(invenType, slot))
    {
        return 1;
    }

    Inven_Item slotItem = user.getCurCharacInvenR()->GetInvenSlot(invenType, slot);

    out = slotItem; // ORIG：0x3d 字节逐字段复制

    if (*(int*)((char*)&out + 0x02) == 0) // m_addInfo(item idx)==0 → 视为空槽失败
    {
        return 1;
    }

    return 0;
