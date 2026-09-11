void CBoosterGage::sub_total_gage(int nGage)
{
    alter();
    m_dwTotalGage -= nGage;
    if (m_dwTotalGage < 0)
        m_dwTotalGage = 0;
}