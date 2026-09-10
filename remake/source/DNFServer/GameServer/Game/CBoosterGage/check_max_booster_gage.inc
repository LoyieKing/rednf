unsigned char CBoosterGage::check_max_booster_gage(unsigned long dwItemIdx)
{
    int nRet = get_booster_point(dwItemIdx);
    if (nRet == 0)
        return 0;
    if (m_dwTotalGage >= G_CDataManager()->m_boosterGage.m_cMaxBoosterGage)
        return 1;
    return 0;
