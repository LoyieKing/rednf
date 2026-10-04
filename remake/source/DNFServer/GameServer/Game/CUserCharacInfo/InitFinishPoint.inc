void CUserCharacInfo::InitFinishPoint()
{
    if (m_selected)
    {
    m_selected->m_finishPoint = 0;
    m_selected->m_finishPointTotal = 0;
    memset(m_selected->m_warAreaKill, 0, sizeof(m_selected->m_warAreaKill));
    ((CUser*)this)->m_historyLog.FPSub(
        0, m_selected->m_finishPoint, m_selected->m_finishPointTotal, (eFPSubReason)1);
    }
}