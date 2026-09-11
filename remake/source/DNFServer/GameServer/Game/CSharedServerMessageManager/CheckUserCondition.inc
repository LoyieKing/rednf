bool CSharedServerMessageManager::CheckUserCondition(CUser* pUser, int nIndex, int& nOut)
{
    int nMapped = m_adapter.GetMappedEventID(nIndex);
    if (nMapped == -1)
        return false;

    int nSlot = 0;
    switch (nMapped)
    {
    case 1:
        break;
    case 3:
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x28d3aa);
        if (nSlot == -1)
            return false;
        break;
    case 0:
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x28d3aa);
        if (nSlot == -1)
            return false;
        break;
    case 2:
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x1d9d);
        if (nSlot == -1)
            return false;
        break;
    default:
        return false;
    }
    return true;
}