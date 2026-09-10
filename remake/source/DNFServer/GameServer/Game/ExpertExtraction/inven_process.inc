bool CExtraction::inven_process(CUser* user, short param,
                                std::vector<std::pair<int, int> >& needItems,
                                std::vector<int>& outSlots)
{
    CInventory* invenW = user->getCurCharacInvenW();
    CInventory copy;
    InventoryMemory mem;
    copy.SetInventoryMemory(&mem);
    copy.setCopy(*(const CInventory*)user->getCurCharacInvenRefR());
    bool deleted = copy.delete_item((INVEN_TYPE)1, (int)param, 1,
                                    (eItemDelReason)0x2d, 0);
    if (!deleted)
    {
        return false;
    }
    int first = needItems[0].first;
    int second = needItems[0].second;
    CItem* item = G_CDataManager()->find_item(first);
    Inven_Item newItem;
    item->make_item(newItem);
    newItem.set_add_info(second);
    newItem.m_addInfo = first;
    int slot = copy.insertItemIntoInventory(newItem, (eItemAddReason)0x35, 0, 1);
    if (slot == -1)
    {
        return false;
    }
    invenW->delete_item((INVEN_TYPE)1, (int)param, 1,
                        (eItemDelReason)0x2d, 1);
    slot = invenW->insertItemIntoInventory(newItem, (eItemAddReason)0x35, 1, 1);
    outSlots.clear();
    outSlots.push_back(slot);
    outSlots.push_back(first);
    outSlots.push_back(second);
    return true;
