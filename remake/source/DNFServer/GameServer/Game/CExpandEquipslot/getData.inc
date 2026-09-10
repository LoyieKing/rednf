void CExpandEquipslot::getData(char* pData) const
{
    SIG_SAVE_EXPAND_CHARAC* pData2 = (SIG_SAVE_EXPAND_CHARAC*)pData;
    int nGroupCount = 0xc;
    int nTotalCount = 0x24;
    int nType = 1;
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
            pData2->m_items[i].setCopy(m_equipSlot[i]);
        }
        else if (nType == 2)
        {
            pData2->m_items[i].setCopy(m_expandEquipSlot[i % 0xc]);
        }
        else if (nType == 3)
        {
            pData2->m_items[i].setCopy(m_expandAvatarSlot[i % 0xc]);
        }
    }
