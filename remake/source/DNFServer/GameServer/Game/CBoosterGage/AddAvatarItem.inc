int CBoosterGage::AddAvatarItem(std::vector<std::pair<int, int> >& itemList,
                                bool bIsFirst, CUser* pUser, const Inven_Item& item,
                                int nCount, bool bFlag, char cType, char cParam,
                                const char* pAgency, eAvatarItemAddReason eReason,
                                int& nRemainDate)
{
    int nSlot = 0;
    int nLoop = 1;
    if (bIsFirst)
    {
        nLoop = G_CDataManager()->m_boosterGage.m_cMultiboxCount;
        eReason = eAvatarItemAddReason_0x16;
    }
    for (int i = 0; i < nLoop; ++i)
    {
        nSlot = pUser->getCurCharacInvenW()->AddAvatarItem(
            item.m_addInfo, nCount, bFlag, 0, -1, pAgency, eReason, 0, 0);
        if (nSlot == -1)
        {
            make_postar(pUser, item, pAgency, nCount);
        }
        else
        {
            time_t tNow = time(0);
            Inven_Item invenItem = pUser->getCurCharacInvenR()->GetInvenSlot(2, nSlot);
            nRemainDate = pUser->getCurCharacInvenR()->GetAvatarItemMgrR()
                              ->GetRemainDate(invenItem.m_addInfo2, (int)tNow);
            itemList.push_back(std::make_pair(nSlot, item.GetItemSpace()));
        }
    }
    return nSlot;
