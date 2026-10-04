void CUserCharacInfo::SetCurCharacChaosDieCount(int count)
{
    if (m_selected)
    {
    m_selected->m_chaosDieCount = count;
    }
}