void BlueMarbleUserInfo::setExp(int exp)
{
    if (m_buff.m_exp * exp >= ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue)
    {
        m_buff.m_exp = ((BM_ScriptMaxBuff*)m_pScript)->m_maxBuffValue;
    }
    else
    {
        m_buff.m_exp = m_buff.m_exp * exp;
    }
