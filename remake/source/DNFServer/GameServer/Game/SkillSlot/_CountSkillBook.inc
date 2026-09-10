void CSkillChanger::_CountSkillBook(Inven_Item& item)
{
    for (int i = 1; i <= g_nStatUpItemIdx[0]; i++)
    {
        if (item.m_addInfo == g_nStatUpItemIdx[i])
        {
            m_data.m_elixerCount[i] = item.m_addInfo2;
            m_data.m_skillBookCount[i] = g_nElixerAbility[i];
        }
    }
