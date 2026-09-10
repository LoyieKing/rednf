bool CBoosterGage::loadData(CUser* pUser, char* pData)
{
    // [b28c-11] NEAR 封顶（52/52 条）：残余 = 虚调用 callee 寄存器 %eax↔%edx
    // 互换（`mov (%eax),%edx; …call *%edx` vs `mov (%eax),%eax; …call *%eax`），
    // N7 族（4.4 间接 call 目标恒 %eax）。
    _reset();
    SIG_BOOSTER_GAGE_DATA* p = (SIG_BOOSTER_GAGE_DATA*)pData;
    m_dwTotalGage = p->m_dwBoosterGage;
    if (CheckDailyScheduleTime(G_CEnvironment()->m_serverEnvir.m_fatigueTime,
                               pUser->GetAccountLastPlayTime(),
                               GlobalData::s_systemTime_.getCurSec()))
    {
        ResetDaily();
        send_data(pUser, 0);
    }
    return true;
