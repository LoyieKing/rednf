void CUserCharacInfo::InitFinishPoint()
{
    if (m_selected)
    {
    m_selected->m_fieldE06 = 0;
    m_selected->m_fieldE0A = 0;
    memset(m_selected->m_warAreaKill, 0, sizeof(m_selected->m_warAreaKill));
    ((CUser*)this)->m_historyLog.FPSub(
        0, m_selected->m_fieldE06, m_selected->m_fieldE0A, (eFPSubReason)1);
    }
}