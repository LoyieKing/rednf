int CBoosterGage::get_booster_point(unsigned long dwItemIdx)
{
    unsigned char cRet = 0;
    cRet = G_CDataManager()->m_boosterGage.getBoosterPoint(dwItemIdx);
    if (cRet == 0)
        return 0;
    return cRet;
}