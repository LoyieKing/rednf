void CCompound_ExtreamDun_Item::after_addItem()                     // ORIG 0x832de18
{
    for (int i = 1; (unsigned int)i < (unsigned int)GROUP_NUM_MAX; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::sort(m_data.m_bindItems[i][j].begin(),
                      m_data.m_bindItems[i][j].end());
        }
    }
    for (int k = 0; k < 4; ++k)
    {
        std::sort(m_data.m_resultItems[k].begin(), m_data.m_resultItems[k].end());
    }
