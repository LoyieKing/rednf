void CUserCharacInfo::setBringUpRewardFlag(bool flag)
{
    if (m_selected)
    {
    m_selected->m_bringUpRewardFlag = flag;
    }
}