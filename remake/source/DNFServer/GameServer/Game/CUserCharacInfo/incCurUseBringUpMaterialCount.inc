void CUserCharacInfo::incCurUseBringUpMaterialCount()
{
    if (m_selected)
    {
    ++m_selected->m_pad1279;
    ++m_selected->m_field127a;
    }
}