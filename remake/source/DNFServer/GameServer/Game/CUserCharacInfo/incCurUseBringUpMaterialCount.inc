void CUserCharacInfo::incCurUseBringUpMaterialCount()
{
    if (m_selected)
    {
    ++CUR->m_curUseBringUpMaterialCount;
    ++CUR->m_curUseBringUpMaterialTotal;
    }
