void CBoosterGage::getData(char* pData) const
{
    SIG_BOOSTER_GAGE_DATA* pData2 = (SIG_BOOSTER_GAGE_DATA*)pData;
    memset(pData2, 0, sizeof(SIG_BOOSTER_GAGE_DATA));
    pData2->m_dwBoosterGage = m_dwTotalGage;
}