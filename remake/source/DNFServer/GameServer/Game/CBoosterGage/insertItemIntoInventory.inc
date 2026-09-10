int CBoosterGage::insertItemIntoInventory(std::vector<std::pair<int, int> >& itemList,
                                          bool bIsFirst, CUser* pUser,
                                          const Inven_Item& item,
                                          eItemAddReason eReason, bool bLog)
{
    int nSlot = 0;
    int nLoop = 1;
    if (bIsFirst)
    {
        nLoop = G_CDataManager()->m_boosterGage.m_cMultiboxCount;
        eReason = eItemAddReason_0x3a;
    }
    for (int i = 0; i < nLoop; ++i)
    {
        nSlot = pUser->getCurCharacInvenW()->insertItemIntoInventory(
            item, eReason, bLog, true);
        if (nSlot < 0)
        {
            nSlot = -1;
            make_postar(pUser, item, NULL, 0);
        }
        else
        {
            itemList.push_back(std::make_pair(nSlot, item.GetItemSpace()));
        }
    }
    return nSlot;
