int CBoosterGage::InsertCreatureItem(std::vector<std::pair<int, int> >& itemList,
                                     bool bIsFirst, CUser* pUser,
                                     const Inven_Item& item, eItemAddReason eReason,
                                     bool bLog, bool bParam, int nA, int nB,
                                     const char* pMailTitle)
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
            item, eReason, bLog, bParam);
        if (nSlot < 0)
        {
            nSlot = -1;
            make_postar(pUser, item, pMailTitle, 0);
        }
        else
        {
            pUser->getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
                (Inven_Item*)&item, nSlot, eReason, nA, nB);
            itemList.push_back(std::make_pair(nSlot, item.GetItemSpace()));
        }
    }
    return nSlot;
