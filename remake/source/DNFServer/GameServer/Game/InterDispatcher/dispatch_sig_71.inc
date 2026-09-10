int Inter_AvatarInsert::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_AvatarInsertPktView* pPkt = reinterpret_cast<Inter_AvatarInsertPktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    const int slot = (int)pPkt->m_8 - 10;
    const unsigned int addInfoKey = pPkt->m_4b;

    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item item = invenR->GetInvenSlot(2, slot);

    CInventory* invenW = user->getCurCharacInvenW();
    WongWork::CAvatarItemMgr* mgr = invenW->GetAvatarItemMgrW();
    mgr->AssignAvatarItemKey(item.m_addInfo2, (int)addInfoKey);
    item.m_addInfo2 = (int)addInfoKey;

    // [b23S3C] P48：ORIG update_item 前重新 call getCurCharacInvenW（共 2 次），不复用 invenW。
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)2, slot, item);
    return 0;
