bool Dispatcher_UseDye::getItem(const CUser& user, int invenType, short slot,
                                Inven_Item& out) const
{
    out = const_cast<CInventory*>(user.getCurCharacInvenR())
              ->GetInvenSlot(invenType, slot);
    // ORIG 0x081e71c0：jne 到 mov $0 岛，fall-through mov $1（非 sete 折叠形）。
    if (out.m_addInfo == 0)
        return true;
    return false;
