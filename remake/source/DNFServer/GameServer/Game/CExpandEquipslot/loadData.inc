bool CExpandEquipslot::loadData(CUser* pUser, char* pData)
{
    _reset();
    SIG_SAVE_EXPAND_CHARAC* pData2 = (SIG_SAVE_EXPAND_CHARAC*)pData;
    int nType = 1;
    int nGroupCount = 0xc;
    int nTotalCount = 0x24;
    for (unsigned int i = 0; i < 0x24; ++i)
    {
        if (i != 0 && (i % 0xc) == 0)
        {
            if (i == 0xc)
            {
                nType = 2;
            }
            else if (0xc < i)
            {
                nType = 3;
            }
        }
        if (nType == 1)
        {
            m_equipSlot[i].setCopy(pData2->m_items[i]);
        }
        else if (nType == 2)
        {
            m_expandEquipSlot[i % 0xc].setCopy(pData2->m_items[i]);
        }
        else if (nType == 3)
        {
            m_expandAvatarSlot[i % 0xc].setCopy(pData2->m_items[i]);
        }
    }
    alter();
    return 1;
