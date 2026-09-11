// [K7 2026-09-11] 同 CBoosterGage.h —— ORIG 调用点/定义体直证返回 bool（体 return 0/1 兼容）。
bool CBoosterGage::check_max_booster_gage(unsigned long dwItemIdx)
{
    int nRet = get_booster_point(dwItemIdx);
    if (nRet == 0)
        return 0;
    if (m_dwTotalGage >= G_CDataManager()->m_boosterGage.m_cMaxBoosterGage)
        return 1;
    return 0;
}