// SIGFIX b28c-L2：返回类型 int→bool（ORIG rodata 串 "bool CInventory::CheckExist…"）。
bool CInventory::CheckExistItemInInventoryOrEquipment(int itemIdx, int& slot, bool& bFound) const
{
    bFound = false;
    for (int i = 3; i <= 8; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            slot = i;
            return 1;
        }
    }
    // ORIG 0x8505322：type 槽 -0x18；enum 槽免 cast（StartEndPos 实参序依赖）。
    Inven_Item::ITEM_TYPE type = (Inven_Item::ITEM_TYPE)GetItemType(itemIdx);
    if (type == 0)
    {
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            // ORIG 0x850532b +0x6d: vararg 形 mov -0x18(%ebp),%eax → 传 type 值。
            0x1ace, "Unknown Type %d", type);
        return 0;
    }
    for (int i = 0; i <= 0x19; i++)
    {
        if (m_invenItem[i].m_addInfo == itemIdx)
        {
            bFound = true;
            slot = i;
            return 1;
        }
    }
    int start = 0;
    int end = 0;
    StartEndPos(type, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == itemIdx)
        {
            slot = i;
            return 1;
        }
    }
    return 0;
}