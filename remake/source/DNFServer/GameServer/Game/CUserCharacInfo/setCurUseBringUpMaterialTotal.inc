void CUserCharacInfo::setCurUseBringUpMaterialTotal(unsigned char total)
{
    if (m_selected)
    {
    CUR->m_curUseBringUpMaterialTotal = total;
    }
}