void CUserCharacInfo::setCurUseBringUpMaterialTotal(unsigned char total)
{
    if (m_selected)
    {
    m_selected->m_useBringUpMaterialTotal = total;
    }
}