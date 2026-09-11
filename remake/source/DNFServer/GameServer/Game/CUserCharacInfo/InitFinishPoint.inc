void CUserCharacInfo::InitFinishPoint()
{
    if (m_selected)
    {
    CUR->m_finishPoint = 0;
    CUR->m_finishPointTotal = 0;
    memset(CUR->m_warAreaKill, 0, sizeof(CUR->m_warAreaKill));
    ((CUser*)this)->m_historyLog.FPSub(
        0, CUR->m_finishPoint, CUR->m_finishPointTotal, (eFPSubReason)1);
    }
}